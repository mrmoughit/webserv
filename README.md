Webserv - 42 Project
🌐 Overview
Webserv is a group project at 42 School focused on building a fully functional HTTP web server in C++ from scratch, following the HTTP/1.1 protocol (RFC 2616). The server must be able to handle multiple client requests concurrently, serve static files, manage configuration parsing, support CGI execution, and more—all without relying on high-level frameworks or external libraries.

This project deepens your understanding of networking, sockets, HTTP protocols, and low-level system programming in a multi-client, real-time environment.

🧠 Key Objectives
Implement a basic yet functional HTTP/1.1 web server

Parse and handle HTTP methods: GET, POST, and DELETE

Serve static files (HTML, CSS, images, etc.)

Execute CGI scripts (like PHP or Python)

Support multiple virtual servers (hosted on different ports or domains)

Proper error handling and status code responses

Manage configuration via a custom .conf file

Use non-blocking sockets and poll()

🛠️ Technologies
Language: C++98

Network API: POSIX sockets

I/O Multiplexing: poll()

the project Structure
.
├── conf/                   # Configuration files
│   └── default.conf
├── src/
│   ├── server/             # Core server logic
│   ├── config/             # Configuration parsing
│   ├── http/               # HTTP request/response handling
│   ├── utils/              # Helper utilities
│   └── main.cpp
├── www/                    # Static site root (web files)
│   └── index.html
├── cgi-bin/                # CGI scripts (Python, PHP, etc.)
├── Makefile
└── README.md


 Configuration
The .conf file lets you define:

Server names and listening ports

Root directories for serving files

Index files

Error pages (e.g., 404, 500)

Allowed HTTP methods

Client body size limits

CGI script handling and routes

Example snippet:

nginx
Copy
Edit
server {
    listen 8080;

    root ./www;
    index index.html;

    error_page 404 /errors/404.html;

    location /cgi-bin/ {
        cgi_pass /usr/bin/python3;
        root ./cgi-bin;
    }
}

✅ Features
 Custom configuration parser

 HTTP/1.1 request parsing

 Response generation

 Chunked transfer encoding

 CGI support (Python, PHP, etc.)

 Autoindex directory listing

 Multiplexed I/O using poll()

 Proper error handling (400, 404, 500, etc.)

 Logging (optional)

🧪 Testing
Compatible with curl, telnet, and web browsers

CGI scripts tested with Python and Bash

Stress tested with multiple clients and tools like ab or wrk

How to run 
1- clone the project 
git clone https://github.com/yourusername/webserv.git
cd webserv
2-buld and run project 
make 
./webserv conf/default.conf
in you browser or postman visit the http://localhost:8080
