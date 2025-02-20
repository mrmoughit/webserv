#include "request.hpp"

#define PORT 5555

void parse_request(const std::string& request_buffer) {
    std::istringstream requestStream(request_buffer);
    std::string line;

    request object;
    std::getline(requestStream, line);
    std::istringstream requestLine(line);
    if (!check_request_line(line))
        return ;

    std::string method, path, version , error;
    requestLine >> method >> path >> version >> error;
    if (error.size() > 0 || !method.size() || !path.size() || !version.size()){
        std::cout << "400 bad request" << std::endl;
        return ;
    }
    object.set_method(method);
    object.set_path(path);
    object.set_version(version);

    if (object.get_method() != "GET" && object.get_method() != "POST" && object.get_method() != "DELETE"){
        if (!is_upper(object.get_method())){
            std::cout << "400 Bad requeste" << std::endl;
            return;
        }
        else{
            std::cout << "405 Not Allowed" << std::endl;
            return ;
        }
    }

    if (object.get_version() != "HTTP/1.1"){
        if (strncmp(object.get_version().c_str() , "HTTP/" , 5) > 0){
            std::cout << "400 bad request" << std::endl;
            return ;
        }
        else{
            std::cout << "505 HTTP Version Not Supported" << std::endl;
            return ;
        }
    }
    std::string pa = object.get_path();
    if (pa[0] != '/'){
        std::cout << "400 bad request" << std::endl;
        return;
    }
    pa = removeslashes(pa);
    out_root_dir(pa);
    char **str = ft_split(pa.c_str() , '/');
    int entry = 0;
    int sorty = 0;
    for (int i = 0; str[i] ; i++){
        if (strcmp(str[i] , "..") == 0)
            sorty++;
        else
            entry++;
    }
    if (sorty > entry){
        std::cout << "400 bad request" << std::endl;
        return;
    }
    object.set_path(pa);
    object.fill_headers_map(requestStream );
  
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed!" << std::endl;
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Binding failed!" << std::endl;
        return -1;
    }
    if (listen(server_fd, 5) == -1) {
        std::cerr << "Listening failed!" << std::endl;
        return -1;
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_fd == -1) {
            std::cerr << "Failed to accept connection!" << std::endl;
            continue; 
        }

        std::cout << "Client connected.\n";
        char buffer[1024] = {0};
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            std::string request(buffer, bytes_received);
            parse_request(request);   
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, client! Your request was received.";
            ssize_t bytes_sent = send(client_fd, response.c_str(), response.size(), 0);
            if (bytes_sent == -1) {
                std::cerr << "Failed to send response!" << std::endl;
            }
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}

