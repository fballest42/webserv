#pragma once
#include <vector>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <stdio.h>      
#include <stdlib.h> 
#include <cstring>  
#include <sstream>
#include <string> 

bool isSpace(char c)
{ return (c == ' ');}

void strip(const std::string& in, std::string& out) 
{ 
    std::string::const_iterator b = in.begin(), e = in.end(); 
    // skipping leading spaces 
    while (isSpace(*b)){ ++b; } if (b != e)
    { 
        // skipping trailing spaces 
        while (isSpace(*(e-1)))
        { --e; } 
    } 
    out.assign(b, e); 
}

int nb_tokens(std::string str, std::string delimiter)
{
    std::istringstream iss(str);
    std::string token;
    int ret=0;
    while (std::getline(iss, token, ' '))
    {
        ret++;
    }
    return ret;
}

std::string  get_token(std::string str, std::string delimiter, int nb)
{
    std::istringstream iss(str);
    std::string token;
    int ret=0;
    while (std::getline(iss, token, ' '))
    {
        //std::cout << token << std::endl;
        if (ret == nb)
            break;
        ret++;
    }
    return token;
}


class Config
{
    private:
        std::string _path;
        int         _fd;
        std::vector<std::string> _vector_cfg;
        int         _workers;
        std::vector<int> _ports;    //listen varios un parametro = port)
        //_server_name       //server_name example.com www.example.com;
        //_root;          //unique root /var/www/example.com; www
        //_cgi;           // varios 2 parameters 
        //_index;         //varios? ? parameter (check if exits root/..)
        //_error_page;   //(varios 2 param (int) (path))

    public:

        Config(char *path);
        void show_config();
        void show_ports(void);
        int get_workers(void);
        

};
Config::Config(char *path) : _path(path) {
    //_workers = 1;
    _fd = open(_path.c_str(), O_RDONLY);
    if (_fd < 0){
        std::cout << "Error: no config file"<< std::endl;
        //throw std::invalid_argument("could not open configuration file : ");
        exit(EXIT_FAILURE);
    }
    int flag = 0;
    char *line;
    std::string s_out ="";
    std::string s_tmp ="";

    while(get_next_line(_fd, &line))
    {
      std::string s(line);
      strip(s, s_out);                          //elimino espacios 
      //
      //pendiente tratar {}      
      //
      if (flag == 0 && !s_out.empty() && s_out.at(0) != '#')
       {
            _vector_cfg.push_back(s_out);
            s_tmp = "";
       }
    }

    // get tokens workers CHECK NUMERO DE TOKENS AND TYPE INTEGER
    int c = 0;
    while(c<_vector_cfg.size())
    {
        if (get_token(_vector_cfg.at(c), " ", 0) == "workers" )   //check number of tokens pendiente
            _workers = (stoi(get_token(_vector_cfg.at(c), " ", 1)));
        
        c++;
    }

    // get tokens port CHECK NUMERO DE TOKENS AND TYPE INTEGER
   c = 0;
    while(c<_vector_cfg.size())
    {
        if (get_token(_vector_cfg.at(c), " ", 0) == "listen" )   //check number of tokens pendiente
            _ports.push_back(stoi(get_token(_vector_cfg.at(c), " ", 1)));
        c++;
    }
}

void Config::show_config(void)
{
    int c = 0;
    while(c<_vector_cfg.size())
    {
        std::cout << _vector_cfg.at(c) << " numbr of tokens: " << nb_tokens(_vector_cfg.at(c)," ") << std::endl;
        c++;
    }
    return;
}

void Config::show_ports(void)
{
    int c = 0;
    while(c < _ports.size())
        std::cout << "ports:" << _ports.at(c++)  << std::endl;
    return;
}

int Config::get_workers(void)
{
    return _workers;
}