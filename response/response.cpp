#include "../webserver.hpp"

std::ifstream & Response::get_fileStream(){
    return *fileStream;
}

void Response::set_fileStream(std::ifstream & object){
    fileStream = &object;
}

Response::~Response(){};
Response::Response(){};