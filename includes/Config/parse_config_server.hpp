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
#include "utils.hpp" 
#include "config.hpp" 
#include "get_next_line.hpp" 

class Parse_config
{
    private:
        std::string                                     _path;
        std::vector< std::vector<std::string> >         _server;
        int                                             _nb_servers;
        int                                             _workers;
        
    public:
        std::vector< Config >                           _configuration;
        
        Parse_config(std::string  path);
        ~Parse_config(){};

        Config get_server(int n){return _configuration[n];};
        bool parse(void);
        int get_nb_servers(){return _nb_servers;};
        void show_config(void);
        void show_config_one(int n);
};

