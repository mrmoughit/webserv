#include "request.hpp"

#define PORT 4444
void get_error_res(std::string &res, int status)
{
    if (status == 400)
    {
        res = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n\
        <html><head><title>400 Bad Request</title></head><body><center><h1>400 Bad Request</h1></center>\
        <hr><center>42 webserv 0.1</center></body></html>";
    }
    else if (status == 405)
    {
        res = "HTTP/1.1 405 Not Allowed\r\nContent-Type: text/html\r\n\r\n\
        <html><head><title>405 Not Allowed</title></head><body><center><h1>405 Not Allowed</h1></center>\
        <hr><center>42 webserv 0.1</center></body></html>";
    }
    else if (status == 505)
    {
        res = "HTTP/1.1 505 HTTP Version Not Supported\r\nContent-Type: text/html\r\n\r\n\
        <html><head><title>505 HTTP Version Not Supported</title></head><body><center><h1>505 HTTP Version Not Supported</h1></center>\
        <hr><center>42 webserv 0.1</center></body></html>";
    }
}

void get_default_index(std::string &res, std::string path)
{
    std::ifstream f;
    if (path == "/")
        f.open("www/index.html");
    else
        f.open(path.c_str());
    std::string line;
    res = "HTTP/1.1 200 ok\r\nContent-Type: text/html\r\n\r\n";
    while (std::getline(f, line))
    {
        res += line;
        // std::cout <<" hada ----------------->" <<  line << std::endl;
    }
}
void parse_request(const std::string &request_buffer, request &object, std::string &res)
{
    res = "";
    std::istringstream requestStream(request_buffer);
    std::string line;
    (void)res;
    std::getline(requestStream, line);
    std::istringstream requestLine(line);
    if (!check_request_line(line))
        return;

    std::string method, path, version, error;
    requestLine >> method >> path >> version >> error;
    if (error.size() > 0 || !method.size() || !path.size() || !version.size())
    {
        // std::cout << "400 bad request" << std::endl;
        get_error_res(res, 400);
        return;
    }
    object.set_method(method);
    object.set_path(path);
    object.set_version(version);

    if (object.get_method() != "GET" && object.get_method() != "POST" && object.get_method() != "DELETE")
    {
        if (!is_upper(object.get_method()))
        {
            // std::cout << "400 Bad requeste" << std::endl;
            get_error_res(res, 400);
            return;
        }
        else
        {
            // std::cout << "405 Not Allowed" << std::endl;
            get_error_res(res, 405);
            return;
        }
    }

    if (object.get_version() != "HTTP/1.1")
    {
        if (strncmp(object.get_version().c_str(), "HTTP/", 5) > 0)
        {
            // std::cout << "400 bad request" << std::endl;
            get_error_res(res, 400);
            return;
        }
        else
        {
            // std::cout << "505 HTTP Version Not Supported" << std::endl;
            get_error_res(res, 505);
            return;
        }
    }
    std::string pa = object.get_path();
    if (pa[0] != '/')
    {
        // std::cout << "400 bad request" << std::endl;
        get_error_res(res, 400);
        return;
    }
    pa = removeslashes(pa);
    if (!out_root_dir(pa, res))
        return;

    object.set_path(pa);

    if (object.fill_headers_map(requestStream, res) == 0)
        return;

    if (object.get_path() == "/")
    {
        get_default_index(res, object.get_path());
        return;
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
            return;
        }
        DIR *dir = opendir(pat.c_str());

        if (dir == NULL)
        {
            std::cerr << "Error opening directory: " << strerror(errno) << std::endl;
            return;
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

    else if (S_ISREG(path_stat.st_mode))
        get_default_index(res, object.get_path());
    else
        res = "HTTP/1.1 404 not found\r\nContent-Type: text/html\r\n\r\n\
    <html><head><title>404 not found</title></head><body><center><h1>404 not found</h1></center>\
    <hr><center>42 webserv 0.1</center></body></html>";
}

int main()
{
    std::string response;
    request object;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        std::cerr << "Socket creation failed!" << std::endl;
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cerr << "Binding failed!" << std::endl;
        return -1;
    }
    if (listen(server_fd, 5) == -1)
    {
        std::cerr << "Listening failed!" << std::endl;
        return -1;
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    while (true)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd == -1)
        {
            std::cerr << "Failed to accept connection!" << std::endl;
            continue;
        }

        std::cout << "Client connected.\n";
        char buffer[1024] = {0};
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received > 0)
        {
            std::string request(buffer, bytes_received);
            parse_request(request, object, response);
            ssize_t bytes_sent = send(client_fd, response.c_str(), response.size(), 0);
            if (bytes_sent == -1)
            {
                std::cerr << "Failed to send response!" << std::endl;
            }
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
