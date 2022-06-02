#pragma once
#include <vector>
#include <map>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <stdio.h>      
#include <stdlib.h> 
#include <cstring>  
#include <sstream>
#include <string> 
#include "custom_exception.hpp"
#include "utils.hpp"


//Calse para guardar la configuración de un servidor
class Config
{
    private:
        std::vector<std::string>            _vector_cfg;
        
        std::vector<int>                    _ports;         //listen varios un parametro = port)
        std::vector<std::string>            _roots;         //varios? ? parameter (check if exits root/..)
        std::vector<std::string>            _server_names; //varios server_name example.com www.example.com;
        std::vector<std::string>            _index;        //varios? ? parameter (check if exits root/..)
        std::vector<std::string>            _autoindex;     //pdte
        std::vector<std::string>            _upload;       // pdte
        std::map<std::string, std::string>  _cgi;
        std::map<std::string, std::string>  _cgi_bin;       //pdte
        std::map<int ,std::string>          _error_pages;
        //std::vector<std::string, std::string > _locations;

    public:

        Config(std::vector<std::string> _vector_cfg);

        int parse(void);
        void show_config();
        void show_index(void);
        void show_autoindex(void);
        void show_ports(void);
        void show_cgi(void);
        void show_cgi_bin(void);
        void show_error_pages(void);
        void show_roots(void);
        void show_server_names(void);
        int port(int p);
      

};
