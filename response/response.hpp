#pragma once 

#include <iostream> 
#include <cstdlib>
#include <map>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <fstream>


class response
{
    private:
        std::ifstream fileStream;
    public:
        std::ifstream & get_fileStream();
        void set_fileStream(std::ifstream & object);
        
};

