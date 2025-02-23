#pragma once 
#include "../webserver.hpp"

class client{
    private:
        int client_id;
        // response response_object;
        // request request_object;
    public:
        void set_client_id(int fd);
        int get_client_id();
        // request  get_request();
        // void  set_request(request & R);
        // response & get_response();
        // void  set_response(response & R);

        client();
        ~client(){};
};