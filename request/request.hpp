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
class request
{
    private:
        std::string method; 
        std::string version; 
        std::string path;
        std::map<std::string,std::string> headers_map;

    public:
        const std::string& get_method();
        const std::string& get_version();
        const std::string& get_path();
        void set_method(std::string & name);
        void set_path(std::string& name);
        void set_version(std::string& name);

        bool fill_headers_map(std::istringstream &obj);

        request();
        ~request();
};

bool check_request_line(std::string & line);
std::string removeslashes(const std::string& line);
bool out_root_dir(std::string line);
bool is_upper(std::string line);
char	**ft_split(char const *s, char c);