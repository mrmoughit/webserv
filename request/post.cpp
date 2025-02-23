#include "request.hpp"



#define BUFFER_SIZE 8192

void handle_post_requst(request & object , std::string res , std::istringstream &requestStream) {
    object.fill_headers_map(requestStream, res);
    std::string line;
    std::map<std::string, std::string> headers;
    std::string body;

    while (std::getline(requestStream, line) && line != "\r") {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 2, line.length() - (colonPos + 3));
            headers[key] = value;
        }
    }
    std::getline(requestStream, body, '\0');
        // int contentLength = 0;
        // if (headers.find("Content-Length") != headers.end()) {
            // contentLength = std::stoi(headers["Content-Length"]);
        // }
    //     if (body.length() >= contentLength) {
    //         body = body.substr(0, contentLength);
    //     }

    //     std::cout << "Received file content:\n" << body << std::endl;


    //     std::ofstream outFile("uploaded_file.txt", std::ios::binary);
    //     if (outFile.is_open()) {
    //         outFile.write(body.c_str(), body.size());
    //         outFile.close();
    //         std::cout << "File saved as 'uploaded_file.txt'" << std::endl;
    //     } else {
    //         std::cerr << "Failed to save file." << std::endl;
    // } else {
    //     std::cerr << "Not a POST request." << std::endl;
    // }
}

