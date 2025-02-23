#pragma once 

#include "../webserver.hpp"

class Client{
    private:
        int client_id;
        Request request_object;
        Response response_object;
    public:
        void set_client_id(int fd);
        int get_client_id();
        Request  get_request();
        void  set_request(Request & R);
        Response & get_response();
        void  set_response(Response & R);

        Client();
        ~Client(){};
};