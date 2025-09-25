# WebServ — HTTP/1.1 Web Server in C++

## Table of Contents

1. [Overview](#overview)  
2. [Features](#features)  
3. [Project Structure](#project-structure)  
4. [Configuration](#configuration)  
5. [Usage & Setup](#usage--setup)  
6. [Architecture & Components](#architecture--components)  
7. [Error Handling](#error-handling)  
8. [Testing & Validation](#testing--validation)  
9. [To-Do & Future Work](#to-do--future-work)  
10. [Acknowledgments & References](#acknowledgments--references)

---

## 1. Overview

**WebServ** is a custom HTTP/1.1 web server implemented in C++ (ISO C++98 style), developed as a project for 42 School. It is built from scratch without external libraries or frameworks.

The goal is to understand and implement:

- Socket programming  
- HTTP protocol  
- CGI execution  
- Asynchronous I/O  
- Configurable virtual servers  

---

## 2. Features

- Supported HTTP methods: `GET`, `POST`, `DELETE`  
- Serve static files (HTML, CSS, images, etc.)  
- Run CGI scripts (Python, PHP)  
- Virtual hosting support  
- Custom `.conf` configuration file  
- Autoindex directory listing  
- Error pages and status handling  
- Non-blocking I/O with `poll()`  
- Basic logging  
- Multiple listening ports

---

## 3. Project Structure

webserv/
├── main.cpp
├── Makefile
├── test.conf
├── cgi.cpp
├── webserver.hpp
├── c_tools/
│ └── tools.cpp
├── client/
│ ├── client.cpp
│ └── client.hpp
├── parsing/
│ ├── Confile.*
│ ├── ServerBlock.*
│ ├── RouteBlock.*
│ └── parsing.cpp
├── request/
│ ├── request.cpp/hpp
│ ├── request_implementation.cpp
│ └── tools.cpp
├── response/
│ ├── response.cpp
│ └── response.hpp
├── server/
│ ├── server.cpp
│ └── server.hpp
├── upload/ # temporary uploaded files
├── www/ # static site content
├── my_site/ # custom CGI & HTML pages
└── pages/, error/ # HTML pages & error templates



---

## 4. Configuration

The server is configured through a custom `.conf` file.

### Sample `test.conf` snippet:

```nginx
server {
  host 127.0.0.1;
  listen 8080;
  server_name mysite.com;

  index index.html;
  root ./www;
  client_max_body_size 2G;

  error_page 404 ./www/pages/404.html;

  location /error {
    root ./my_site;
    allowed_methods DELETE GET POST;
    autoindex on;
    index index.html;
    cgi_extension .py;
    client_body_temp_path ./upload;
  }
}

You can define:

Listening ports and IP addresses

Server names

Root directories & index files

Allowed methods per location

Error pages

CGI script handling


5. Usage & Setup
🧱 Build
make

🚀 Run
./webserv test.conf

If no configuration is given, a default server is used.

Then open your browser or terminal and visit:

http://localhost:8080

Test Tools

curl

telnet

Web browsers

Benchmarks (ab, wrk)

6. Architecture & Components
✅ Request Handling

Parses HTTP request line, headers, body

Supports Transfer-Encoding: chunked

Handles query strings, cookies, form data

📝 Response Generation

Sets status codes, content types, headers

Serves files or autoindexed directory listings

Matches routes via longest prefix

Handles connection persistence

⚙️ CGI Execution

Recognizes .py, .php scripts

Spawns subprocess to run script

Reads CGI output and includes it in response

Manages timeouts and environment variables

⚡ I/O Multiplexing

Uses poll() for concurrent client connections

All sockets and pipes are non-blocking

Graceful handling of timeouts and disconnections

🌐 Routing & Virtual Servers

Configurable via server and location blocks

Multiple ports and domains supported

Route matching is hierarchical

7. Error Handling

Custom error pages for:

400 Bad Request

403 Forbidden

404 Not Found

405 Method Not Allowed

413 Payload Too Large

415 Unsupported Media Type

500 Internal Server Error

502 Bad Gateway

504 Gateway Timeout

505 HTTP Version Not Supported

Errors are logged and mapped to user-friendly HTML pages.

8. Testing & Validation

Manual and automated tests cover:

Static file serving

CGI script execution

Large file uploads

Chunked requests

Invalid method/route handling

Autoindex generation

Sample Tools

curl -X POST ...

ab -n 1000 -c 50 ...

Custom CGI test scripts (Python/PHP)

9. To-Do & Future Work

Improve chunked request edge case handling

Properly manage Connection headers

Add PUT, HEAD method support

Improve CGI error handling

Add logging output to file

Integrate SSL (HTTPS)

Add full test suite

10. Acknowledgments & References

Inspired by Nginx & Apache

Based on RFC 2616 — HTTP/1.1 Specification

42 Network Project

Tools used: curl, telnet, ab, Valgrind

Made with 🧠, 💻, and ☕ by students at 42.




