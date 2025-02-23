#pragma once

#include "../webserver.hpp"

class Response
{
    private:
        std::ifstream *fileStream;
    public:
        std::ifstream & get_fileStream();
        void set_fileStream(std::ifstream & object);
        Response();
        ~Response();
};
