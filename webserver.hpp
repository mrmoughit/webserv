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

#include "response/response.hpp"
#include "request/request.hpp"
#include "client/client.hpp"

bool check_request_line(std::string & line);
std::string removeslashes(const std::string& line);
bool out_root_dir(std::string &pa , std::string &res);
bool is_upper(std::string line);
char	**ft_split(char const *s, char c);
void get_error_res(std::string &res , int status);
std::string fill_response(std::ifstream& fileStream,  std::string& filePath);
std::string  parse_request(const std::string &request_buffer, Request &object, std::ifstream& fileStream);
