#pragma once


#include <iostream> 
#include <cstdlib>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include "../webserver.hpp"
class request
{
    private:
        std::string method; 
        std::string version; 
        std::string path;
        std::map<std::string,std::string> headers_map;
        char *s_request;
        
    public:
        const std::string& get_method();
        const std::string& get_version();
        const std::string& get_path();
        void set_method(std::string & name);
        void set_path(std::string& name);
        void set_version(std::string& name);
        void set_s_request(char * reques);
        char * get_s_request();
        bool fill_headers_map(std::istringstream &obj , std::string &res);

        request();
        ~request();
};
