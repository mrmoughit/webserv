#include "request.hpp"
#include "../webserver.hpp"

#define PORT 4445


std::string  parse_request(const std::string &request_buffer, request &object, std::ifstream& fileStream)
{
    std::string res;
    std::istringstream requestStream(request_buffer);
    std::string line;
    std::getline(requestStream, line);
    std::istringstream requestLine(line);
    std::cout << request_buffer << std::endl ;
    if (!check_request_line(line))
        return res;
    
    std::string method, path, version, error;
    requestLine >> method >> path >> version >> error;
    if (error.size() > 0 || !method.size() || !path.size() || !version.size())
    {
        // std::cout << "400 bad request" << std::endl;
        get_error_res(res, 400);
        return res;
    }
    object.set_method(method);
    object.set_path(path);
    object.set_version(version);

    if (object.get_method() != "GET" && object.get_method() != "POST" && object.get_method() != "DELETE")
    {
        if (!is_upper(object.get_method()))
        {
            get_error_res(res, 400);
            return res;
        }
        else
        {
            get_error_res(res, 405);
            return res;
        }
    }
    if (object.get_method() == "POST"){
        std::cout << "here" << std::endl;
        // handle_post_requst(object , res , requestStream);
        return res;
    }
    if (object.get_version() != "HTTP/1.1")
    {
        if (strncmp(object.get_version().c_str(), "HTTP/", 5) > 0)
        {
            // std::cout << "400 bad request" << std::endl;
            get_error_res(res, 400);
            return res;
        }
        else
        {
            // std::cout << "505 HTTP Version Not Supported" << std::endl;
            get_error_res(res, 505);
            return res;
        }
    }
    std::string pa = object.get_path();
    if (pa[0] != '/')
    {
        // std::cout << "400 bad request" << std::endl;
        get_error_res(res, 400);
        return res;
    }
    pa = removeslashes(pa);
    if (!out_root_dir(pa, res))
        return res;

    object.set_path(pa);

    if (object.fill_headers_map(requestStream, res) == 0)
        return res;

    if (object.get_path() == "/")
    {
        std::string p = "/index.html";
        object.set_path(p);
    }
    std::string pat = "www/" + object.get_path().substr(1);
    object.set_path(pat);
    struct stat path_stat;
    if (stat(object.get_path().c_str(), &path_stat) == -1)
    {
        std::cerr << "Error: stat field" << std::endl;
        res = "HTTP/1.1 404 not found\r\nContent-Type: text/html\r\n\r\n\
        <html><head><title>404 not found</title></head><body><center><h1>404 not found</h1></center>\
        <hr><center>42 webserv 0.1</center></body></html>";
    }

    else if (S_ISDIR(path_stat.st_mode))
    {
        if (access(pat.c_str(), R_OK | W_OK | X_OK) == -1)
        {
            res = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\n\r\n\
                <html><head><title>403 Forbidden</title></head><body><center><h1>403 Forbidden</h1></center>\
                <hr><center>42 webserv 0.1</center></body></html>";
            return res;
        }
        DIR *dir = opendir(pat.c_str());

        if (dir == NULL)
        {
            std::cerr << "Error opening directory: " << strerror(errno) << std::endl;
            return res;
        }

        struct dirent *entry;
        res = "HTTP/1.1 200 OK\r\n";
        res += "Content-Type: text/html; charset=UTF-8\r\n";
        res += "Connection: close\r\n";
        res += "\r\n";

        res += "<html>\n<head>\n<title>Found Files</title>\n</head>\n<body>\n";
        res += "<h1>Found Files in Directory</h1>\n<ul>\n";
        while ((entry = readdir(dir)) != NULL)
        {
            std::string fileName = entry->d_name;
            res += "<li><a href=\"/" + pat.substr(4) +"/" + fileName + "\">" + fileName + "</a></li>\n";
            // 4 must be change by root dir size 
        }
        res += "</ul>\n</body>\n</html>\n";
        closedir(dir);
    }

    else if (S_ISREG(path_stat.st_mode)){
        res = fill_response(fileStream , pat);
    }
    else
        res = "HTTP/1.1 404 not found\r\nContent-Type: text/html\r\n\r\n\
            <html><head><title>404 not found</title></head><body><center><h1>404 not found</h1></center>\
            <hr><center>42 webserv 0.1</center></body></html>";
    return res;
}


bool setupSocket(int& server_fd, struct sockaddr_in& server_addr) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
        return false;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Setsockopt failed: " << strerror(errno) << std::endl;
        return false;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        return false;
    }

    if (listen(server_fd, 10) == -1) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}


void handleClient(int client_fd , request &object) {
    char recv_buffer[1024] = {0};
    ssize_t bytes_received = recv(client_fd, recv_buffer, sizeof(recv_buffer), 0);
    
    if (bytes_received > 0) {
        std::string request(recv_buffer, bytes_received);

        std::ifstream fileStream;
        std::string response = parse_request(request ,object ,  fileStream);
        
        if (send(client_fd, response.c_str(), response.length(), 0) == -1) {
            std::cerr << "Failed to send headers: " << strerror(errno) << std::endl;
            return;
        }

        char send_buffer[8192];
        size_t total_sent = 0;
        
        while (fileStream.good() && !fileStream.eof()) {
            fileStream.read(send_buffer, sizeof(send_buffer));
            size_t bytes_read = fileStream.gcount();  
            if (bytes_read == 0) 
                break;    
           size_t bytes_sent = 0;
            while (bytes_sent < bytes_read) {
                ssize_t result = send(client_fd, send_buffer + bytes_sent,bytes_read - bytes_sent,0);
                
                if (result <= 0) {
                    std::cerr << "Send failed: " << strerror(errno) << std::endl;
                    fileStream.close();
                    return;
                }
                
                bytes_sent += result;
                total_sent += result;
            }
            
            std::cout << "Progress: " << total_sent << " bytes sent\r" << std::flush;
        }
        fileStream.close();
        std::cout << "\nTotal sent: " << total_sent << " bytes\n";
    }
}

int main() {
    int server_fd = -1;
    struct sockaddr_in server_addr;
    request object;
    
    if (!setupSocket(server_fd, server_addr)) {
        return 1;
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd == -1) {
            std::cerr << "Accept failed: " << strerror(errno) << std::endl;
            continue;
        }

        int flag = 1;
        setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

        std::cout << "Client connected\n";
        handleClient(client_fd , object);
        close(client_fd);
        std::cout << "Connection closed\n\n";
    }
    close(server_fd);
    return 0;
}

