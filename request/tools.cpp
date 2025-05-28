#include "../webserver.hpp"

void trim(std::string &str)
{
    size_t start = 0;
    size_t end = str.size();

    while (start < end && std::isspace(str[start]))
    {
        ++start;
    }

    while (end > start && std::isspace(str[end - 1]))
    {
        --end;
    }

    str = str.substr(start, end - start);
}

std::string getContentType(std::string filePath)
{

    std::map<std::string, std::string> mimeTypes;

    mimeTypes.insert(std::make_pair(".html", "text/html"));
    mimeTypes.insert(std::make_pair(".css", "text/css"));
    mimeTypes.insert(std::make_pair(".js", "application/javascript"));
    mimeTypes.insert(std::make_pair(".json", "application/json"));
    mimeTypes.insert(std::make_pair(".jpg", "image/jpeg"));
    mimeTypes.insert(std::make_pair(".jpeg", "image/jpeg"));
    mimeTypes.insert(std::make_pair(".png", "image/png"));
    mimeTypes.insert(std::make_pair(".gif", "image/gif"));
    mimeTypes.insert(std::make_pair(".mp4", "video/mp4"));
    mimeTypes.insert(std::make_pair(".pdf", "application/pdf"));
    mimeTypes.insert(std::make_pair(".txt", "text/plain"));
    mimeTypes.insert(std::make_pair(".zip", "application/zip"));
    mimeTypes.insert(std::make_pair(".mp3", "audio/mpeg"));

    size_t dotPos = filePath.find_last_of(".");
    if (dotPos != std::string::npos)
    {
        std::string ext = filePath.substr(dotPos);
        for (size_t i = 0; i < ext.length(); i++)
        {
            ext[i] = tolower(ext[i]);
        }

        if (mimeTypes.count(ext))
        {
            return mimeTypes[ext];
        }
    }
    return "";
}

std::string fill_response(std::ifstream &fileStream, std::string &filePath, Client &client, int status)
{

    fileStream.close();
    fileStream.open(filePath.c_str(), std::ios::ate);

    if (!fileStream.is_open())
    {
        std::cout << "cat't open " << filePath << std::endl;
        return "";
    }

    std::streampos fileSize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    client.get_response().set_response_status(200);
    std::ostringstream response;

    response << "HTTP/1.1 ";

    int redirection = client.get_request().redirection;

    if (redirection != -1)
    {
        if (redirection == 301)
        {
            response << "301 Moved Permanently\r\n";
            response << "Location: ";
            response << client.get_request().Location;
            response << " \r\n";
        }
        else if (redirection == 302)
        {
            response << "302 Found\r\n";
            response << "Location: ";
            response << client.get_request().Location;
            response << " \r\n";
        }
    }
    else
    {
        response << status;
        response << " OK\r\n";
    }

    response << "Content-Type:" + getContentType(filePath) + "\r\n";
    response << "Content-Length: " << fileSize << "\r\n";
    response << "Accept-Ranges: bytes\r\n";
    if (client.get_Alive())
        response << "Connection: keep-alive\r\n";
    else
        response << "Connection: close\r\n";
    response << "\r\n";

    return response.str();
}

bool check_if_have_cgi(Client &client)
{

    bool bol = false;
    std::string path = client.get_request().get_path();
    size_t pos = path.find_last_of('.');

    if (pos != std::string::npos)
    {
        std::string ex = path.substr(pos);
        if (ex == ".php" || ex == ".py")
        {
            if (client.server_client_obj.is_location_url > -1)
            {
                std::vector<std::string> vec = client.server_client_obj.get_routes()[client.server_client_obj.is_location_url].get_cgi_ext();
                for (size_t i = 0; i < vec.size(); i++)
                {
                    static std::string body_cgi;
                    if (vec[i] == ex)
                    {
                        bol = true;
                        if (client.get_request().get_method() == "POST")
                        {
                            std::string content_type = client.get_request().get_map_values("Content-Type");
                            std::string chunk = client.get_request().get_map_values("Transfer-Encoding");

                            if (chunk == "chunked" && (content_type == "application/x-www-form-urlencoded" || content_type == "text/plain"))
                            {
                                std::string tmp = chunked_for_cgi(&client);
                                if (!tmp.empty())
                                    body_cgi += tmp;
                                if (client.get_all_recv())
                                {
                                    cgi_handler(client, body_cgi, path);
                                    body_cgi = "";
                                }
                            }
                            else if (content_type == "application/x-www-form-urlencoded" || content_type == "text/plain")
                            {
                                body_cgi += client.get_request().get_s_request();
                                if (client.get_request().get_content_length() == body_cgi.size())
                                {
                                    cgi_handler(client, body_cgi, path);
                                    body_cgi = "";
                                    client.set_all_recv(true);
                                }
                            }
                            else
                                return (set_response_error(&client, 405), 1);
                        }
                        else if (client.get_request().get_path() == "GET")
                        {
                            client.set_all_recv(true);
                            cgi_handler(client, "", path);
                        }
                        else
                        {
                            return (set_response_error(&client, 405), 1);
                            return bol;
                        }
                        if (client.get_Alive())
                        {
                            client.get_response().set_response_index(true);
                            client.get_response().set_response_status(200);
                        }
                    }
                }
            }
            else
            {
                bol = true;
                static std::string body_cgi;
                if (client.get_request().get_method() == "POST")
                {
                    std::string content_type = client.get_request().get_map_values("Content-Type");
                    std::string chunk = client.get_request().get_map_values("Transfer-Encoding");

                    if (chunk == "chunked" && (content_type == "application/x-www-form-urlencoded" || content_type == "text/plain"))
                    {
                        std::string tmp = chunked_for_cgi(&client);
                        if (!tmp.empty())
                            body_cgi += tmp;
                        if (client.get_all_recv())
                        {
                            cgi_handler(client, body_cgi, path);
                            body_cgi = "";
                        }
                    }
                    else if (content_type == "application/x-www-form-urlencoded" || content_type == "text/plain")
                    {
                        body_cgi += client.get_request().get_s_request();
                        if (client.get_request().get_content_length() == body_cgi.size())
                        {
                            cgi_handler(client, body_cgi, path);
                            body_cgi = "";
                            client.set_all_recv(true);
                        }
                    }
                    else
                        return (set_response_error(&client, 415), 1);
                }
                else if (client.get_request().get_method() == "GET")
                {
                    cgi_handler(client, "", path);
                    client.set_all_recv(true);
                }
                else
                {
                    return (set_response_error(&client, 405), 1);
                    return bol;
                }
                if (client.get_Alive())
                {
                    client.get_response().set_response_index(true);
                    client.get_response().set_response_status(200);
                }
            }
        }
    }
    return bol;
}