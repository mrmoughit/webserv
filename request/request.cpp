#include "../webserver.hpp"

Request::Request(){};
Request::~Request(){};

const std::string & Request::get_method(){
    return method;
}

const std::string & Request::get_path(){
    return path;
}

const std::string & Request::get_version(){
    return  version; 
}

void Request::set_method(std::string & name){
    method = name;
}

void    Request::set_s_request(char * req){
    s_request = req;
}


void Request::set_path(std::string & name){
    path = name;
}

char * Request::get_s_request(){
    return s_request;
}

void Request::set_version(std::string & name){
    version = name;
}


bool is_allowed_char(char c){
    if (c != '/' && c != '?' && c != '=' && c != '&' && c != '#' && c != '-' && c != '_' && c != ':' && c != '.' && c != ' ' && (int)c != 13\
        && c != '(' && c != ')' && c != '[' && c != ']')
        return false;
    return true;
}


bool check_request_line(std::string & line){
    int i = 0;
    while(line[i]){
        if (!isalnum(line[i]) && !is_allowed_char(line[i])){
            std::cout << "400 bad request ||" << line[i] << " (ASCII: " << (int)line[i] << ")" << std::endl;
            return false;
        }
        i++;
    }
    return true;
}


std::string removeslashes(const std::string& line) {
    std::string result;
    for (size_t i = 0; i < line.size(); ++i) {
        result += line[i];
        if (line[i] == '/') {
            while (i + 1 < line.size() && line[i + 1] == '/') {
                ++i;
            }
        }
    }
    return result;
}


bool Request::fill_headers_map(std::istringstream &ob , std::string &res){
    std::string line, key, value;
    while (std::getline(ob, line)) {
        if (line.empty()) continue;
        std::size_t pos = line.find(':');

        if (pos != std::string::npos){
            key = line.substr(0, pos);
            value = line.substr(pos + 1);
        }
        else
            key = line.substr(pos + 1);
        if (key.empty()){
            // std::cout << "400 Bad requeste 1" << std::endl;
            get_error_res(res , 400);
            headers_map.clear();
            return false;
        }
        if ((key[0] >= 9 && key[0] < 13) || key[0] == 32){
            // std::cout << "400 Bad requeste 2 "<< "|" << (int)key[0] << "|"  << std::endl;
            get_error_res(res , 400);
            headers_map.clear();
            return false;
        }
        headers_map[key] = value;
    }
    return true;
}

bool out_root_dir(std::string &pa , std::string &res){
    char **str = ft_split(pa.c_str(), '/');
    int entry = 0;
    int sorty = 0;
    for (int i = 0; str[i]; i++)
    {
        if (strcmp(str[i], "..") == 0)
            sorty++;
        else
            entry++;
        if (sorty > entry)
        {
            get_error_res(res, 400);
            return false;
        }
    }
    std::vector<std::string> vec;

    for (int i = 0 ; str[i] ; i++){
        vec.push_back(str[i]);
        if (strcmp(str[i] , "..") == 0){
            vec.pop_back();
            vec.pop_back();
        }
    }
    pa = "/";
    std::ostringstream oss;
    for (size_t i = 0; i < vec.size(); ++i) {
        oss << vec[i];
        if (i != vec.size() - 1) {
            oss << "/"; 
        }
    }
    pa += oss.str();
    return true;
}

bool is_upper(std::string line){
    for (std::size_t i = 0; i < line.size() ; i++){
        if (!(line[i] >= 'A' && line[i] <= 'Z' ))
            return false;
    }
    return true;
}
