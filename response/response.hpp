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
class response
{
    private:
        std::ifstream *fileStream;
    public:
        std::ifstream & get_fileStream();
        void set_fileStream(std::ifstream & object);
        response();
        ~response();
};
