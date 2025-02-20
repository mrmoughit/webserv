#include "request.hpp"

request::request(){};
request::~request(){};

const std::string & request::get_method(){
    return method;
}

const std::string & request::get_path(){
    return path;
}

const std::string & request::get_version(){
    return  version; 
}

void request::set_method(std::string & name){
    method = name;
}

void request::set_path(std::string & name){
    path = name;
}

void request::set_version(std::string & name){
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
            std::cout << "400 bad request " << line[i] << " (ASCII: " << (int)line[i] << ")" << std::endl;
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


bool request::fill_headers_map(std::istringstream &ob){
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
            std::cout << "400 Bad requeste 1" << std::endl;
            headers_map.clear();
            return false;
        }
        if ((key[0] >= 9 && key[0] < 13) || key[0] == 32){
            std::cout << "400 Bad requeste 2 "<< "|" << (int)key[0] << "|"  << std::endl;
            headers_map.clear();
            return false;
        }
        headers_map[key] = value;
    }
    return true;
}

bool out_root_dir(std::string line){
    (void)line;
    return false;
}

bool is_upper(std::string line){
    for (std::size_t i = 0; i < line.size() ; i++){
        if (!(line[i] >= 'A' && line[i] <= 'Z' ))
            return false;
    }
    return true;
}