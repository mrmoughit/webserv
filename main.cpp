#include "webserver.hpp"

int get_parts(char **av, std::vector <std::string>& parts)
{
    std::ifstream infile(av[1]);
    if (!infile.is_open())
    {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }
    std::string line;
    std::vector<std::string> lines;
    while(std::getline(infile, line, '\n'))
        lines.push_back(line);
    std::cout << "size : " << lines.size() << std::endl;
    if (lines.empty())
        return (std::cerr << "Error empty file!" << std::endl, 1);
    size_t i = 0;
    size_t j;
    size_t last_full_line = 0;
    while ( i < lines.size()) //while loop to skip last empty lines to get the real last line
    {
        // std::cout << "servive: " << "i: " << i << "line: " << lines[i] << std::endl;
        if (!lines[i].empty() && check_empt(lines[i].c_str()))
        {
            // std::cout << "not empty" << std::endl;
            last_full_line = i;
            j  = 0;
            while (j < lines[i].length())
            {
                if (lines[i][j] == '#')
                {
                    std::cout << "line: " << lines[i] << std::endl;
                    lines[i] = lines[i].substr(0, j);
                    std::cout << "line after substr: " << lines[i] << std::endl;
                    break;
                }
                j++;
            }
        }
        i++;
    }
    if (last_full_line == 0)
        return (std::cerr << "Error empty file!" << std::endl, 1);
    std::string newlastline = trimstr(lines[last_full_line]);
    if (newlastline.find(';') != std::string::npos) //check line l3amer
        return (std::cout << "Error ';' at the end of file!" << std::endl, 1);
    std::string lines_string;
    i = 0;
    while (i < lines.size())
    {
        lines_string.append(lines[i]);
        i++;
    }
    std::istringstream iss(lines_string);
    std::string part;
    while (std::getline(iss, part, ';'))
        parts.push_back(part);
    for (size_t y = 0; y < parts.size(); y++)
    {
        if (parts[y].empty() || !check_empt(parts[y].c_str()))
            return (std::cout << "Error Double ';' in configuration file" << std::endl, 1);
    }
    infile.close();
    return 0;
}




int main(int ac, char **av)
{

    (void)av;
    Server S1;
    if (ac == 2)
    {

        std::vector <std::string> parts;
        if (get_parts(av, parts))
             return 1;
        Confile conf;

        conf.set_server(parts);
        if (conf.status == false)
            return (1);
        if (conf.status == false)
            return (std::cout << "Fixe config file and try again!" << std::endl, 1);
        S1.number_of_servers = conf.number_of_server;
        std::vector <ServerBlock> servers = conf.get_server();
       
        S1.server_block_obj = servers;
        for (size_t i = 0; i < conf.number_of_server ; i++)
        {
            S1.addServerConfig(i, servers[i].get_host(), servers[i].get_port(), servers[i].get_server_names());
        }
    }
    else
    {
        std::vector <ServerBlock> servers;
        ServerBlock default_server;
        default_server.set_dafault_data();

        servers.push_back(default_server);
        if(set_default_page())
            return 1;
        S1.number_of_servers = 1;
        S1.server_block_obj = servers;
        S1.addServerConfig(0, servers[0].get_host(), servers[0].get_port(), servers[0].get_server_names());
        // std::cout << "Usage: ./webserv <config_file>" << std::endl;
    }
    S1.startServer();
   
} 
