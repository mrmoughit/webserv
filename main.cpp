#include "webserver.hpp"


void trim_non_printable(std::string &str)
{
    size_t start = 0;
    while (start < str.length() && !std::isprint(str[start]))
        ++start;

    size_t end = str.length();
    while (end > start && !std::isprint(str[end - 1]))
        --end;
    str = str.substr(start, end - start);
}

int hex_to_int(const std::string &hexStr)
{
    int result = 0;
    for (size_t i = 0; i < hexStr.length(); ++i)
    {
        char c = hexStr[i];
        if (c >= '0' && c <= '9')
            result = result * 16 + (c - '0');
        else if (c >= 'a' && c <= 'f')
            result = result * 16 + (c - 'a' + 10);
        else if (c >= 'A' && c <= 'F')
            result = result * 16 + (c - 'A' + 10);
    }
    return result;
}

std::string generate_file_names(const std::string &extension)
{
    static int index;
    std::string name;
    while (1)
    {
        std::ostringstream ss;

        ss << "file" << index << "." << extension;
        name = ss.str();
        if (access((root + "/" + name).c_str(), F_OK) != 0)
            return name;
        index++;
    }
    return NULL;
}

void chunked(Client &client)
{
    static std::string request;
    request += client.get_request().get_s_request();
    static std::string result;
    std::string line;
    while (true) {
        size_t pos = request.find("\r\n");
        
        if (pos == std::string::npos) {
            return  ;
        }

        line = request.substr(0 , pos + 2);
        size_t size = hex_to_int(line);
        if (size == 0){
            client.set_all_recv(true);
            client.get_request().set_s_request(result);
            request  = result = "";
            hanlde_post_request(client);
            return ;
        }
        std::string tmp = request.substr(pos + 2);
        if (tmp.size() < size){
            return ;
        }
        request = request.substr(pos + 2);
        result += request.substr(0 , size);
        
        request = request.substr(size + 2);
    }
}

int check_if_have_new_boundary(std::string &buffer, std::string boundary, Client &client)
{
    boundary = "--" + boundary;

    size_t pos = buffer.find(boundary);

    if (pos == std::string::npos)
        return -1;

    size_t last_Boundary = pos + boundary.size();

    if (last_Boundary + 2 <= buffer.size() &&
        buffer[last_Boundary] == '-' && buffer[last_Boundary + 1] == '-')
        client.set_all_recv(true);

    return static_cast<int>(pos);
}

void fill_data_boudary(const std::string &tmp, Client &clinet)
{
    std::istringstream ss(tmp);
    std::string line;
    std::ofstream file;

    std::getline(ss, line);
    std::string key;
    if (line.find("Content-Disposition:") != std::string::npos)
    {
        size_t name_pos = line.find("name=\"");
        if (name_pos != std::string::npos)
        {
            size_t name_start = name_pos + 6;
            size_t name_end = line.find("\"", name_start);
            if (name_end != std::string::npos)
                key = line.substr(name_start, name_end - name_start);
            else
            {
                std::cerr << "Invalid boundary format 1" << std::endl;
                return;
            }
            size_t filename_pos = line.find("filename=\"");
            if (filename_pos != std::string::npos)
            {
                filename_pos += 10;
                size_t file_name_end = line.find("\"", filename_pos);
                if (file_name_end == std::string::npos)
                {
                    std::cout << "error" << std::endl;
                    exit(55);
                }
                std::string filename = line.substr(filename_pos, file_name_end - filename_pos);
                // std::cout << filename << std::endl;
                filename  = root + "/" + filename;
                file.open(filename.c_str());
                std::getline(ss, line);
                std::getline(ss, line);
                while (1)
                {
                    char c;
                    line = "";
                    while (ss.get(c))
                        line += c;
                    if (line.empty())
                        break;
                    file << line << std::flush;
                }
            }
            std::getline(ss, line);
            while (1)
            {
                char c;
                line = "";
                while (ss.get(c))
                {
                    line += c;
                }
                if (line.empty())
                    break;
                clinet.fill_map(key, line);
            }
        }
        else
        {
            std::cerr << "Invalid boundary format 3" << std::endl;
            return;
        }
    }
}

void boundary(Client &client)
{
    static std::string buffer;
    static int i = 0;
    static std::string boundary;
    std::string tmp;

    buffer += client.get_request().get_s_request();

    if (i == 0)
    {
        std::istringstream ss(buffer);
        std::getline(ss, tmp);
        size_t pos = tmp.find_first_not_of("-");
        size_t end = tmp.find("\r");
        boundary = tmp.substr(pos, end - pos);
        pos = buffer.find("\n");
        buffer = buffer.substr(pos + 1);
        i++;
    }

    while (true)
    {
        int index = check_if_have_new_boundary(buffer, boundary, client);
        if (index == -1)
        {
            client.print_map();
            break;
        }
        else if (index == 0)
        {
            buffer = buffer.substr(boundary.size() + 4);

        }
        else
        {
            tmp = buffer.substr(0, index - 2);
            buffer = buffer.substr(index);
            fill_data_boudary(tmp, client);
        }
    }
    buffer =  boundary = "";
    i = 0;
}





void handle_boundary_chanked(Client &client)
{
    static std::string request;
    request += client.get_request().get_s_request();
    static std::string result;
    std::string line;
    while (true) {
        size_t pos = request.find("\r\n");
        
        if (pos == std::string::npos) {
            return  ;
        }

        line = request.substr(0 , pos + 2);
        size_t size = hex_to_int(line);
        if (size == 0){
            client.set_all_recv(true);
            client.get_request().set_s_request(result);
            request  = result = "";
            boundary(client);
            return ;
        }
        std::string tmp = request.substr(pos + 2);
        if (tmp.size() < size){
            return ;
        }
        request = request.substr(pos + 2);
        result += request.substr(0 , size);
        
        request = request.substr(size + 2);
    }
}


int main(int ac, char **av)
{
    (void)ac;
    (void)av;
    try
    {
        Server S1;
        S1.startServer();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}