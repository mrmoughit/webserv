#include "../webserver.hpp"

Client::Client() : client_id(-1) {
}

int  Client::get_client_id(){
    return client_id;
}

void  Client::set_client_id(int fd){
    client_id = fd;
}

Request & Client::get_request(){
    return request_object;
}

void  Client::set_request(Request & R){

    request_object = R;
}

Response&  Client::get_response(){
    return response_object;
}

void Client::set_response(Response & R) {
        response_object = R;
}