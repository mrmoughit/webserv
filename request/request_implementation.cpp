#include "../webserver.hpp"

std::string status_400 = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>400 </title><style>body{font-family:\"Arial\",sans-serif;background-color:#f4f4f4;color:#333;margin:0;padding:0;display:flex;justify-content:center;align-items:center;height:100vh}.error-container{text-align:center;max-width:600px;padding:40px;background-color:#ffffff;box-shadow:0 4px 10px rgba(0,0,0,0.1);border-radius:8px}h1{font-size:100px;margin:0;color:#e74c3c}p{font-size:18px;margin-top:20px}a{color:#3498db;text-decoration:none}a:hover{text-decoration:underline}</style></head><body><div class=\"error-container\"><h1>400</h1><p>Oops! Bad request.</p><p><a href=\"/\">Go back to homepage</a></p></div></body></html>";
std::string status_404 = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>404 </title><style>body{font-family:\"Arial\",sans-serif;background-color:#f4f4f4;color:#333;margin:0;padding:0;display:flex;justify-content:center;align-items:center;height:100vh}.error-container{text-align:center;max-width:600px;padding:40px;background-color:#ffffff;box-shadow:0 4px 10px rgba(0,0,0,0.1);border-radius:8px}h1{font-size:100px;margin:0;color:#e74c3c}p{font-size:18px;margin-top:20px}a{color:#3498db;text-decoration:none}a:hover{text-decoration:underline}</style></head><body><div class=\"error-container\"><h1>404</h1><p>Oops! The page you are looking for could not be found.</p><p><a href=\"/\">Go back to homepage</a></p></div></body></html>";
std::string status_403 = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>403 </title><style>body{font-family:\"Arial\",sans-serif;background-color:#f4f4f4;color:#333;margin:0;padding:0;display:flex;justify-content:center;align-items:center;height:100vh}.error-container{text-align:center;max-width:600px;padding:40px;background-color:#ffffff;box-shadow:0 4px 10px rgba(0,0,0,0.1);border-radius:8px}h1{font-size:100px;margin:0;color:#e74c3c}p{font-size:18px;margin-top:20px}a{color:#3498db;text-decoration:none}a:hover{text-decoration:underline}</style></head><body><div class=\"error-container\"><h1>403</h1><p>Oops! Forbiden.</p><p><a href=\"/\">Go back to homepage</a></p></div></body></html>";
std::string status_200 = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>200 </title><style>body{font-family:\"Arial\",sans-serif;background-color:#f4f4f4;color:#333;margin:0;padding:0;display:flex;justify-content:center;align-items:center;height:100vh}.error-container{text-align:center;max-width:600px;padding:40px;background-color:#ffffff;box-shadow:0 4px 10px rgba(0,0,0,0.1);border-radius:8px}h1{font-size:100px;margin:0;color:#e74c3c}p{font-size:18px;margin-top:20px}a{color:#3498db;text-decoration:none}a:hover{text-decoration:underline}</style></head><body><div class=\"error-container\"><h1>200</h1><p>Oops! Succes.</p><p><a href=\"/\">Go back to homepage</a></p></div></body></html>";
std::string status_405 = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>405 </title><style>body{font-family:\"Arial\",sans-serif;background-color:#f4f4f4;color:#333;margin:0;padding:0;display:flex;justify-content:center;align-items:center;height:100vh}.error-container{text-align:center;max-width:600px;padding:40px;background-color:#ffffff;box-shadow:0 4px 10px rgba(0,0,0,0.1);border-radius:8px}h1{font-size:100px;margin:0;color:#e74c3c}p{font-size:18px;margin-top:20px}a{color:#3498db;text-decoration:none}a:hover{text-decoration:underline}</style></head><body><div class=\"error-container\"><h1>405</h1><p>Oops! Not allawed method.</p><p><a href=\"/\">Go back to homepage</a></p></div></body></html>";
std::string status_500 = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>500 </title><style>body{font-family:\"Arial\",sans-serif;background-color:#f4f4f4;color:#333;margin:0;padding:0;display:flex;justify-content:center;align-items:center;height:100vh}.error-container{text-align:center;max-width:600px;padding:40px;background-color:#ffffff;box-shadow:0 4px 10px rgba(0,0,0,0.1);border-radius:8px}h1{font-size:100px;margin:0;color:#e74c3c}p{font-size:18px;margin-top:20px}a{color:#3498db;text-decoration:none}a:hover{text-decoration:underline}</style></head><body><div class=\"error-container\"><h1>500</h1><p>Internal server error.</p><p><a href=\"/\">Go back to homepage</a></p></div></body></html>";

void set_response_error(Client *client, int status)
{
    std::string error_path = client->server_client_obj.find_error_page_path(status);
    int red = client->get_request().redirection;


    if (error_path == "NULL")
    {
        std::string res = "HTTP/1.1 ";
        if (red != -1)
        {
            if (red == 301)
            {
                res += "301 Moved Permanently\r\n";
                res += "Location: ";
                res += client->get_request().Location;
                res += " \r\n";
            }
            else if (red == 302)
            {
                res += "302 Found\r\n";
                res += "Location: ";
                res += client->get_request().Location;
                res += " \r\n";
            }
        }
        std::ostringstream content_length;
        std::ostringstream oss;
        oss << status;
        std::string str = oss.str();


        std::string string;
        if (status == 400)
            string = status_400;
        else if (status == 404)
            string = status_404;
        else if (status == 403)
            string = status_403;
        else if (status == 200)
            string = status_200;
        else if (status == 405)
            string = status_405;
        else if (status == 500)
            string = status_500;

        res += str;
        content_length << string.size();
        str = content_length.str();

        res += " OK\r\n";
        res += "Content-Type: text/html; charset=UTF-8\r\n";
        res += "Content-Length: " + str + "\r\n";
        if (client->get_Alive())
            res += "Connection: keep-alive\r\n";
        else
            res += "Connection: close\r\n";
        res += "\r\n";
        res += string;
        
        client->get_response().set_response_status(status);
        client->get_response().set_response(res);
        client->get_response().set_response_index(true);
        
        return;
    }

    std::string res = fill_response(client->get_response().get_fileStream(), error_path, *client, status);
    if (res.empty())
    {
        res = "HTTP/1.1 500 OK\r\n";
        res += "Content-Type: text/html; charset=UTF-8\r\n";
        if (client->get_Alive())
            res += "Connection: keep-alive\r\n";
        else
            res += "Connection: close\r\n";
        res += "\r\n";
        res += status_500;
    }
    client->get_response().set_response_status(status);
    client->get_response().set_response(res);
    client->get_response().set_response_index(true);
}

void parse_request(Client &client)
{
    client.get_request().set_parse_index(true);
    client.set_Alive(false);

    std::string &res = client.get_response().get_response();

    std::string requestData = client.get_request().get_s_request();
    std::string line;

    std::string error_path;
    size_t bodyStart = requestData.find("\r\n\r\n");
    if (bodyStart == std::string::npos)
    {
        bodyStart = requestData.size() - 4;
        if ((int)bodyStart < 0)
        {
            set_response_error(&client, 400);
            return;
        }
    }
    std::string new_request = requestData.substr(bodyStart + 4);
    client.get_request().set_s_request(new_request);
    requestData = requestData.substr(0, bodyStart + 4);

    std::istringstream requestLine(requestData);
    std::getline(requestLine, line);
    std::istringstream Line(line);

    if (!check_request_line(line))
    {
        set_response_error(&client, 400);
        return;
    }

    std::string method, path, version, error;
    Line >> method >> path >> version >> error;
    if (error.size() > 0 || !method.size() || !path.size() || !version.size())
    {
        set_response_error(&client, 400);
        return;
    }

    client.get_request().set_method(method);
    client.get_request().set_path(path);
    client.get_request().set_version(version);

    if (client.get_request().get_method() != "GET" && client.get_request().get_method() != "POST" && client.get_request().get_method() != "DELETE")
    {
        if (!is_upper(client.get_request().get_method()))
        {
            set_response_error(&client, 400);
            return;
        }
        else
        {
            set_response_error(&client, 405);
            return;
        }
    }

    if (client.get_request().get_version() != "HTTP/1.1")
    {
        if (strncmp(client.get_request().get_version().c_str(), "HTTP/", 5) > 0)
        {
            set_response_error(&client, 400);
            return;
        }
        else
        {
            set_response_error(&client, 505);
            return;
        }
    }

    std::string pa = client.get_request().get_path();
    if (pa[0] != '/')
    {
        set_response_error(&client, 400);
        return;
    }
    pa = removeslashes(pa);
    if (!out_root_dir(pa, res, client))
    {
        client.get_response().set_response(res);
        client.get_response().set_response_index(true);
        return;
    }
    client.get_request().set_path(pa);
    if (client.get_request().fill_headers_map(requestLine, res, client) == 0)
    {
        client.get_response().set_response_index(true);
        return;
    }
    std::string tmp = client.get_request().get_map_values("Content-Length");

    std::string tmp1 = pa;

    pa = client.server_client_obj.is_location_path(pa);

    if (tmp1[0] == '/' && pa.size() == 0)
    {
        if (tmp1.size() == 1)
            pa = client.server_client_obj.get_server_root();
        else
            pa = client.server_client_obj.get_server_root() + "/" + &tmp1[1];
    }

    if (pa.size() > 0)
        client.get_request().set_path(pa);

    if (!client.server_client_obj.is_valid_method(pa, method))
    {
        set_response_error(&client, 405);
        return;
    }

    if (method == "POST")
    {
        std::istringstream ss(tmp);
        size_t size;
        ss >> size;
        client.get_request().set_content_length(size);
        if (ss.fail() || size == 0)
        {
            set_response_error(&client, 400);
            return;
        }
        if (size > client.server_client_obj.get_client_body_size())
        {
            set_response_error(&client, 413);
            return;
        }
    }

    pa = client.get_request().get_path();
    check_if_have_redirection(&client);
    if (client.get_response().get_response_index())
        return;

}



void handle_delete_request(std::string path)
{
    struct stat buf;

    if (stat(path.c_str(), &buf) == -1)
    {
        throw std::runtime_error("Invalid path");
    }
    if (S_ISDIR(buf.st_mode))
    {
        DIR *dir = opendir(path.c_str());
        if (dir == NULL)
        {
            throw std::runtime_error("cannot open dir ");
        }
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (std::strcmp(entry->d_name, ".") == 0 || std::strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }
            std::string fullPath = std::string(path) + "/" + entry->d_name;
            handle_delete_request(fullPath);
        }
        if (remove(path.c_str()))
        {
            throw std::runtime_error("remove field , can't remove dir ");
        }
        closedir(dir);
    }
    else
    {
        if (remove(path.c_str()) != 0)
        {
            throw std::runtime_error("remove field , can't remove file ");
        }
    }
}

void check_request(Client &client)
{
    client.server_client_obj.is_location_url = -1;

    
    if (!client.get_request().get_parse_index())
        parse_request(client);

    if (client.get_response().get_response_index())
    {
        client.set_all_recv(true);
        return;
    }

    if (check_if_have_cgi(client))
        return ;
    
    if (client.get_response().get_response_index())
    {
        client.set_all_recv(true);
        return;
    }

    const std::string method = client.get_request().get_method();
    const std::string content_type = client.get_request().get_map_values("Content-Type");
    const std::string transfer_encoding = client.get_request().get_map_values("Transfer-Encoding");

    if (method == "GET")
    {
        response_to_get(client);
        client.set_all_recv(true);
        return;
    }

    if (method == "POST")
    {
        set_response_error(&client, 201);
        client.get_response().set_response_index(false);

        std::string check = transfer_encoding;

        if (content_type.find("boundary=") != std::string::npos && check == "chunked")
            handle_boundary_chanked(client);

        else if (content_type.find("boundary=") != std::string::npos)
            boundary(client);

        else if (check == "chunked")
            chunked(client);

        else if (content_type == "application/x-www-form-urlencoded")
            handle_x_www_form_urlencoded(client);
        else
            hanlde_post_request(client);
    }
    else if (method == "DELETE")
    {
        client.set_all_recv(true);
        std::string path = client.get_request().get_path();
        try
        {
            handle_delete_request(path);
            set_response_error(&client, 204);
        }
        catch (const std::exception &e)
        {
            set_response_error(&client, 404);
        }
    }

}
