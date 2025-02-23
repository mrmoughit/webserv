#pragma once

#include "../webserver.hpp"

class Request
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
        void set_s_request(char * req);
        char * get_s_request();
        bool fill_headers_map(std::istringstream &obj , std::string &res);

        Request();
        ~Request();
};
