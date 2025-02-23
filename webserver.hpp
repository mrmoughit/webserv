#pragma once

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
// void handle_post_requst(request & object , std::string res , std::istringstream &requestStream);