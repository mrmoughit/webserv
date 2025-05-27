#include "./webserver.hpp"
#define CGI_TIMEOUT 5




int check_status(std::string header)
{
    std::vector <std::string> words = get_words(header);
    std::cout << "word 1: " << words[0] << std::endl;
    if (!words.empty())
    {
        if (words[0] == "Status:")
        {
            if (words.size() >= 2)
            {
                std::stringstream  ss(words[1]);
                int status;
                ss >> status;
                return status;
            }
        }
    }
    return 0;
}

int check_content(std::string& content)
{
    std::string delimiter = "\r\n\r\n";
    size_t pos = content.find(delimiter);
        if (pos != std::string::npos) {
        // Extract from the end of the delimiter to the end of the string
        std::string header = content.substr(0, pos);
        std::cout << std::endl << "=> header: " << header << "$$" << std::endl;
        std::string body = content.substr(pos + delimiter.length());
        content = body;
        std::cout << "Body:\n" << body << std::endl;
        return (check_status(header));
    }
    return 0;
}


int check_extension(std::string full_path)
{
	size_t  i = 0;
	std::vector <std::string> cgi_ext;
	cgi_ext.push_back("py");
	cgi_ext.push_back("php");
	if (full_path.size() >= 4)
	{
		size_t pos = full_path.size() - 4;
		std::string extension = full_path.substr(pos);
		if (extension != ".php")
		{
			extension = extension.substr(1);
			if (extension == ".py")
			{
				while (i < cgi_ext.size())
				{
					if (cgi_ext[i] == "py")
						return 2;
					i++;
				}
				return 0;
			}
			else
				return 0;
		}
		else
		{
			while (i < cgi_ext.size())
			{
				if (cgi_ext[i] == "php")
					return 1;
				i++;
			}
			return 0;
		}
		return 0;
	}
	return 0;
}

int exec_script(std::string full_path, char *envp[], const char* interpreter,  Client &client, const std::string& post_data)
{
    printf("Executing script: %s\n", full_path.c_str());
	char *argv[] = {(char *)interpreter, (char *) full_path.c_str(), NULL};

  int fd[2];
   int input_fd[2];
    if (pipe(fd) == -1) {
        std::cerr << "Pipe creation failed: " << strerror(errno) << std::endl;
        return -1;
    }


    // Create input pipe if POST data exists
    bool has_post_data = !post_data.empty();
    if (has_post_data) {
        if (pipe(input_fd) == -1) {
            std::cerr << "Input pipe creation failed" << std::endl;
            close(fd[0]);
            close(fd[1]);
            return -1;
        }
    }



    // Set pipe to non-blocking mode
    int flags = fcntl(fd[0], F_GETFL, 0);
    fcntl(fd[0], F_SETFL, flags | O_NONBLOCK);
    
    pid_t pid;
    if ((pid = fork()) == -1) {
        std::cerr << "Fork failed: " << strerror(errno) << std::endl;
        close(fd[0]);
        close(fd[1]);
        if (has_post_data) {
            close(input_fd[0]);
            close(input_fd[1]);
        }
        return -1;
    }
    
    if (pid == 0) {
        // Child process
        close(fd[0]);

        if (has_post_data) {
            close(input_fd[1]);  // Close write end of input pipe
            if (dup2(input_fd[0], STDIN_FILENO) == -1) {
                std::cerr << "dup2 for stdin failed" << std::endl;
                close(input_fd[0]);
                close(fd[1]);
                exit(1);
            }
            close(input_fd[0]);
        }


        if (dup2(fd[1], STDOUT_FILENO) == -1) {
            std::cerr << "dup2 failed: " << strerror(errno) << std::endl;
            close(fd[1]);
            exit(1);
        }
        
        close(fd[1]);
        execve(argv[0], argv, envp);
        // If execve returns, it failed
        std::cerr << "execve failed: " << strerror(errno) << std::endl;
        exit(1);
    }
    
    // Parent process
    close(fd[1]);
	
        // Send POST data to child process if needed
    if (has_post_data) {
        close(input_fd[0]);  // Close read end of input pipe
        
        // Write POST data to child's stdin
        ssize_t written = 0;
        size_t total_written = 0;
        size_t data_size = post_data.size();
        bool write_error = false;
        
        while (total_written < data_size && !write_error) {
            written = write(input_fd[1], post_data.c_str() + total_written, data_size - total_written);
            if (written > 0) {
                total_written += written;
            } else if (written == 0) {
                // No data written, try again
                continue;
            } else {
                // Error occurred
                std::cerr << "Failed to write POST data" << std::endl;
                write_error = true;
                break;
            }
        }
        
        // Close the pipe to signal EOF to the child process
        close(input_fd[1]);
    }




	struct pollfd pfd;
    pfd.fd = fd[0];
    pfd.events = POLLIN;
    
    char buffer[10000];
	std::string content;
    // ssize_t bytes_read = 0;
    bool timeout_occurred = false;
    
    // Start the timer
    time_t start_time = time(NULL);
    
    // Read with timeout handling and output buffer management
    while (true) 
    {
        // Check if we've exceeded our timeout
        if (time(NULL) - start_time > CGI_TIMEOUT) {
            std::cerr << "CGI script execution timed out after " << CGI_TIMEOUT << " seconds" << std::endl;
            kill(pid, SIGTERM);  // Try to terminate gracefully first
            usleep(100000);      // Wait a bit
            
            // Check if process ended
            int status;
            if (waitpid(pid, &status, WNOHANG) == 0) {
                // Process still running, force kill
                kill(pid, SIGKILL);
            }
            
            timeout_occurred = true;
            break;
        }
        
        // Poll with a short timeout to keep checking our execution time
        int poll_result = poll(&pfd, 1, 100);  // 100ms timeout for poll
        
        if (poll_result == -1) {
            // Poll error
            std::cerr << "Poll error" << std::endl;
            break;
        } else if (poll_result > 0) {
            // Data available to read
            ssize_t n = read(fd[0], buffer, sizeof(buffer));
            if (n > 0) {
                // Data read successfully
                content.append(buffer, n);
            } else if (n == 0) {
                // End of file
                break;
            }
             else 
            {
                    std::cerr << "Read error" << std::endl; 
                    break;
            }
        }
        
        int status;
        pid_t result = waitpid(pid, &status, WNOHANG);
        if (result == pid) {

            break;
        }
    }
    
    close(fd[0]);

    int status = 0;
    if (!timeout_occurred) {
        waitpid(pid, &status, 0);
    }

    if (timeout_occurred) {
        return (set_response_error(&client , 504), 3);
    } else if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) == 1) {
            return (set_response_error(&client , 500) , 1);
        } else if (WEXITSTATUS(status) == 2) {
            return (set_response_error(&client , 404), 1);
        }
    } else if (WIFSIGNALED(status)) {
        return (set_response_error(&client , 500), 1);
    }
    
    if (check_extension(full_path) == 1)
    {
        status = check_content(content);
        if (status == 500)
        return (set_response_error(&client , 500), 1);
        if (status == 404)
            return (set_response_error(&client , 404) , 1);
    }

    std::string &str = client.get_response().get_response();
    
    str = content ;

    client.get_response().set_response(str);

    return 0;
}

std::string convert_to_up(const std::string &key)
{
    std::string result;
    for (size_t i = 0; i < key.length(); ++i) {
        if (key[i] == '-') {
            result += '_';
        } else {
            result += std::toupper(key[i]);
        }
    }
    return result;
}

std::vector<char*> get_env( std::map<std::string,std::string> &map, std::string &full_path, std::string &method , std::string &sn)
{




    std::vector<char*> env_vector;
    std::map<std::string, std::string>::iterator it;
    for (it  = map.begin(); it != map.end(); ++it)
    {
        std::string key = convert_to_up(it->first);
        if (key == "CONTENT_TYPE" || key == "CONTENT_LENGTH")
        {
           std::string combined = key + "=" + it->second; 
           char *var = new char[combined.size() + 1];
           std::strcpy(var , combined.c_str());
           env_vector.push_back(var);
        }
    }


    const char *redirection = "REDIRECT_STATUS=200";
    char *var2 = new char[std::strlen(redirection) + 1];
    std::strcpy(var2, redirection);
    env_vector.push_back(var2);
    
    const char *getway = "GATEWAY_INTERFACE=CGI/1.1";
    char *var3 = new char[std::strlen(getway) + 1];
    std::strcpy(var3, getway);
    env_vector.push_back(var3);
    
    std::string request_method = "REQUEST_METHOD=" + method;
    char *var4 = new char[ request_method.size() + 1];
    std::strcpy(var4, request_method.c_str());
    env_vector.push_back(var4);
    
    std::string script_filename = "SCRIPT_FILENAME=" + full_path;
    char *var5 = new char[script_filename.size() + 1];
    std::strcpy(var5, script_filename.c_str());
    env_vector.push_back(var5);

    std::string script_name = "SCRIPT_NAME=" + sn;
    char *var6 = new char[script_name.size() + 1];
    std::strcpy(var6, script_name.c_str());
    env_vector.push_back(var6);

    env_vector.push_back(NULL);

    return env_vector;
}

int cgi_handler(Client &client , std::string body , std::string &sn)
{   
    std::map<std::string,std::string> map = client.get_request().get_headers_map();
    
	std::string full_path = client.get_request().get_path();
	std::string method = client.get_request().get_method();

    std::vector<char*> env_vec = get_env(map, full_path, method , sn);
    char** env = &env_vec[0];


	int code = check_extension(full_path);
	const char* interpreter;
	if (code > 0)
	{
		if (code == 1)
		{
			// change redirect status from envp to 1; 
			interpreter = "/usr/bin/php-cgi";
		}
		else if (code == 2)
			interpreter = "/usr/bin/python3";
		int status = exec_script(full_path, env, interpreter, client, body);

        for (size_t i = 0; i < env_vec.size() - 1; ++i)
        {
            delete[] env_vec[i];
        }

		return status;
	}
	else
		std::cout << "serve it as regular file" << std::endl;
	return 0;
}
