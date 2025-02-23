#include "response.hpp"

std::ifstream & response::get_fileStream(){
    return *fileStream;
}

void response::set_fileStream(std::ifstream & object){
    fileStream = &object;
}

response::~response(){};
response::response(){};