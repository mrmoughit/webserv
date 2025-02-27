#include "webserver.hpp"

#define PORT 4444

bool setupSocket(int &server_fd, struct sockaddr_in &server_addr)
{
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
        return false;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        std::cerr << "Setsockopt failed: " << strerror(errno) << std::endl;
        return false;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        return false;
    }

    if (listen(server_fd, 10) == -1)
    {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

void handleClient(int client_fd, Client &client)
{
    char request[2000];
    memset(request, 0,2000);
    ssize_t bytes_received = 0;
    std::ifstream fileStream;
    Request req;
    Response res;
    std::string response;

    client.set_response(res);
    res.set_fileStream(fileStream);
    res.set_response(response);
    client.set_request(req);

    std::ofstream file;
    file.open("file.txt");
    if (!file)
        std::cout << "error" << std::endl;
    while ((bytes_received = recv(client_fd, request, 2000 , 0)) > 0)
    {
        std::string tmp(request, bytes_received);
        req.set_s_request(tmp);
        check_request(client);
        // std::cout << req.get_s_request();
        memset(request, 0, 2000);
    }

    if (send(client_fd, client.get_response().get_response().c_str(), client.get_response().get_response().length(), 0) == -1)
    {
        std::cerr << "Failed to send headers: " << strerror(errno) << std::endl;
        return;
    }

    char send_buffer[8192];
    size_t total_sent = 0;

    while (fileStream.good() && !fileStream.eof())
    {
        fileStream.read(send_buffer, sizeof(send_buffer));
        size_t bytes_read = fileStream.gcount();
        if (bytes_read == 0)
            break;

        size_t bytes_sent = 0;
        while (bytes_sent < bytes_read)
        {
            ssize_t result = send(client_fd, send_buffer + bytes_sent, bytes_read - bytes_sent, 0);

            if (result <= 0)
            {
                fileStream.close();
                return;
            }

            bytes_sent += result;
            total_sent += result;
        }
    }

    fileStream.close();
}

int main()
{
    int server_fd = -1;
    struct sockaddr_in server_addr;
    Client client;

    if (!setupSocket(server_fd, server_addr))
    {
        return 1;
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    while (true)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd == -1)
        {
            std::cerr << "Accept failed: " << strerror(errno) << std::endl;
            continue;
        }

        int flag = 1;
        setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

        handleClient(client_fd, client);
        close(client_fd);
    }
    close(server_fd);
    return 0;
}
