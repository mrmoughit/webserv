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

/// /////////////////////////// chanked
static int size = 0;
static int writed = 0;
std::ofstream file1;

int hex_to_int(const std::string &hexStr)
{
    int result = 0;
    for (size_t i = 0; i < hexStr.length(); ++i)
    {
        char c = hexStr[i];
        if (c >= '0' && c <= '9')
            result = result * 16 + (c - '0');
        else if (c >= 'a' && c <= 'f')
            result = result * 16 + (c - 'a' + 10);
        else if (c >= 'A' && c <= 'F')
            result = result * 16 + (c - 'A' + 10);
    }
    return result;
}

void chunked(std::string request)
{
    static bool first_time = true;
    if (first_time)
    {
        file1.open("test.png");
        if (!file1.is_open())
        {
            std::cerr << "can't open file" << std::endl;
            exit(1);
        }
        size_t start = request.find("\r\n\r\n");
        if (start != std::string::npos)
        {
            request = request.substr(start + 4);
        }
        else
        {
            std::cerr << "no body int his fucking request" << std::endl;
            exit(1);
        }
        first_time = false;
    }

    size_t i = 0;
    std::string chunk_size;

    const int get_chunk_size = 0;
    const int read_from_chunk = 1;
    const int chunk_end = 2;
    int state;
    if (size == 0)
        state = get_chunk_size;
    else
        state = read_from_chunk;

    while (i < request.length())
    {
        if (state == get_chunk_size)
        {
            if (request[i] == '\r' && i + 1 < request.length() && request[i + 1] == '\n')
            {
                size = hex_to_int(chunk_size);
                chunk_size = "";
                i += 2;

                if (size == 0)
                    break;
                state = read_from_chunk;
                writed = 0;
            }
            else
            {
                chunk_size += request[i];
                i++;
            }
        }
        else if (state == read_from_chunk)
        {
            file1 << request[i] << std::flush;
            i++;
            writed++;

            if (writed >= size)
                state = chunk_end;
        }
        else if (state == chunk_end)
        {
            if (request[i] == '\r' && i + 1 < request.length() && request[i + 1] == '\n')
            {
                i += 2;
                state = get_chunk_size;
            }
            else
            {
                std::cerr << "the chunk data must be end by \r\n" << std::endl;
                exit (0);
            }
        }
    }
}

/// /////////////////////////// chanked

void handleClient(int client_fd, Client &client)
{
    char request[5000];
    memset(request, 0, 5000);
    ssize_t bytes_received = 0;
    std::ifstream fileStream;
    Request req;
    Response res;
    std::string response;

    client.set_response(res);
    res.set_fileStream(fileStream);
    res.set_response(response);
    client.set_request(req);
    while ((bytes_received = recv(client_fd, request, 5000, 0)) > 0)
    {
        std::string tmp(request, bytes_received);
        req.set_s_request(tmp);
        // check_request(client);
        // chunked(tmp);
        // std::cout << req.get_s_request();
        std::cout << tmp;
        memset(request, 0, 5000);
        // break;
    }
    // std::cout << "here" << std::endl;
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
