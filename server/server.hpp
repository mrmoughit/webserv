#pragma once 
#include "../webserver.hpp"
#include "../parsing/ServerBlock.hpp"
#include "../parsing/Confile.hpp"
#include <poll.h>
#include <fcntl.h>
#include <string>
#include <vector>
#include <sstream>

#define DEFAULT_PORT 8080
#define MAX_CLIENTS 128

// Structure to hold server configuration
struct ServerConfig {
    int server_index;
    std::string host;
    int port;
    int fd;
    std::string server_name;
    struct sockaddr_in addr;
    
    // Constructor with default values
    ServerConfig() : server_index(0), host("localhost"), port(DEFAULT_PORT), fd(-1), server_name("default") {}
    
    // Constructor with parameters
    ServerConfig(const std::string& h, int p, std::string sn = "default") : 
        server_index(0), host(h), port(p), fd(-1), server_name(sn) {}
};

class Server {
private:
    // Server properties
    std::vector<ServerConfig> server_configs;
    
    // Client management
    std::vector<Client> clients;
    
    // Polling structures
    std::vector<pollfd> pollfds;           // All file descriptors (servers + clients)
    std::vector<pollfd> pollfds_clients;   // Only client file descriptors
    std::vector<pollfd> pollfds_servers;   // Only server file descriptors
    
public:
    std::vector<ServerBlock> server_block_obj;
    size_t number_of_servers;
    
    // Constructors/Destructors
    Server();
    ~Server();

    // Configuration methods
    void addServerConfig(const std::string& host, int port, std::string server_name = "default");
    ServerConfig& getServerConfig(size_t index);
    size_t getServerCount() const;
    
    // Server management
    void initializeServers();
    int createServer(ServerConfig& config);
    int acceptClient(int server_fd, ServerBlock& server_block_obj);
    void closeServer();
    
    // Main server loop
    void startServer();
    
    // Client handling
    void handleClientWrite(size_t index);
    void closeClientConnection(size_t index);

    // Helper methods
    void getClientIndexByFd(int fd, size_t& client_index);
    int getServerIndexByFd(int fd);
    int getServerIndexByHostPortName(const std::string& host, int port, const std::string& server_name);
    ServerBlock get_ServerConfByIndex(int fd_server);
};