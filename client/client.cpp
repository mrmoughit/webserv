#include "../webserver.hpp"

client::client() : client_id(-1) {
}

int  client::get_client_id(){
    return client_id;
}

void  client::set_client_id(int fd){
    client_id = fd;
}

// request  client::get_request(){
//     return request_object;
// }

// void  client::set_request(request & R){

//     request_object = R;
// }

// response&  client::get_response(){
//     return response_object;
// }

// void client::set_response(response & R) {
//         response_object = R;
// }