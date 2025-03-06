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
                std::cerr << "the chunk data must be end by \r\n"
                          << std::endl;
                exit(0);
            }
        }
    }
}

void boundary(const std::string &request)
{
    static std::string buffer = "";
    static bool processing = false;
    static std::map<std::string, std::string> form_data;
    static std::ofstream file;
    static std::string current_key;
    static std::string current_filename;
    static bool in_file_content = false;
    static bool in_header = false;
    static std::string boundary;


    buffer += request;


    if (!processing) {
        int pos = buffer.find("\r\n\r\n");
        if (pos == (int)std::string::npos)
            return;


        std::string headers = buffer.substr(0, pos);
        size_t boundary_pos = headers.find("boundary=");
        if (boundary_pos == std::string::npos)
            return; 
            
        boundary_pos += 9; 
        size_t boundary_end = headers.find("\r\n", boundary_pos);
        if (boundary_end == std::string::npos)
            boundary_end = headers.length();
            
        boundary = "--" + headers.substr(boundary_pos, boundary_end - boundary_pos);
        

        buffer = buffer.substr(pos + 4);
        processing = true;
        in_header = true;
    }


    size_t line_end;
    while ((line_end = buffer.find("\r\n")) != std::string::npos) {
        std::string line = buffer.substr(0, line_end);
        buffer = buffer.substr(line_end + 2); 

        if (line.find(boundary) != std::string::npos) {
            if (in_file_content) {
                file.close();
                in_file_content = false;
            }
            
            if (line.find(boundary + "--") != std::string::npos) {

                processing = false;
                buffer.clear();
                form_data.clear();
                return;
            }
            
            in_header = true;
            current_key = "";
            continue;
        }

        if (in_header) {
            if (line.find("Content-Disposition:") != std::string::npos) {
                size_t name_pos = line.find("name=\"");
                if (name_pos != std::string::npos) {
                    size_t name_start = name_pos + 6;
                    size_t name_end = line.find("\"", name_start);
                    if (name_end != std::string::npos) {
                        current_key = line.substr(name_start, name_end - name_start);
                    }
                }
                
                size_t filename_pos = line.find("filename=\"");
                if (filename_pos != std::string::npos) {
                    size_t filename_start = filename_pos + 10;
                    size_t filename_end = line.find("\"", filename_start);
                    if (filename_end != std::string::npos) {
                        current_filename = line.substr(filename_start, filename_end - filename_start);
                    }
                }
            }
            else if (line.find("Content-Type:") != std::string::npos) {
                size_t type_pos = line.find("/");
                if (type_pos != std::string::npos) {
                    std::string extension = line.substr(type_pos + 1);

                    size_t param_pos = extension.find(";");
                    if (param_pos != std::string::npos) {
                        extension = extension.substr(0, param_pos);
                    }
                    
                    if (current_filename.empty()) {
                        current_filename = "file." + extension;
                    }
                }
            }
            else if (line.empty()) {
                in_header = false;
                if (!current_filename.empty()) {
                    file.open(current_filename.c_str(), std::ios::binary);
                    in_file_content = true;
                }
            }
        }
        else if (in_file_content) {
            file << line << "\r\n";
        }
        else if (!current_key.empty()) {
            form_data[current_key] = line;
            std::cout << current_key << " : " << line << std::endl;
            current_key = "";
            in_header = true;
        }
    }
}

void handleClient(int client_fd, Client &client)
{
    char request[1000];
    memset(request, 0, 1000);
    ssize_t bytes_received = 0;
    std::ifstream fileStream;
    Request req;
    Response res;
    std::string response;

    client.set_response(res);
    res.set_fileStream(fileStream);
    res.set_response(response);
    client.set_request(req);
    while ((bytes_received = recv(client_fd, request, 1000, 0)) > 0)
    {
        std::string tmp(request, bytes_received);
        req.set_s_request(tmp);
        check_request(client);
        // boundary(tmp);
        // chunked(tmp);

        // std::cout << tmp;
        memset(request, 0, 1000);
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
