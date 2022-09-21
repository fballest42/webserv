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
#include "logger.hpp"


//Calse para guardar la configuración de un servidor
class Config
{
    friend class Request;
    friend class Response;
    private:
        std::vector<std::string>            _vector_cfg;
        std::vector<int>                    _ports;         //listen varios un parametro = port)
        std::string                         _root;         //solo uno  (check if exits root/..)
        std::vector<std::string>            _server_names; //varios server_name example.com www.example.com;
        std::vector<std::string>            _index;        //varios (check if exits root/..)
        std::string                         _autoindex;     //pdte is BOOL 
        std::string                         _upload;       // pdte
        std::map<std::string, std::string>  _cgi;
        std::string                         _cgi_bin;       //pdte
        std::map<int ,std::string>          _error_pages;
        long                                 _client_max_body_size; 
        std::map<std::string, std::vector<std::string> >   _locations;
        std::vector<std::string>            _limits_except;  

    public:
        int                                 _nb;            //number of server
        Config(std::vector<std::string> _vector_cfg, int nb);
        Config(void);
        void set_Config(std::vector<std::string> vector_cfg, int nb);
        std::string get_root(void);
        std::string get_index(void);
        std::string get_path_error_page(int code);
        std::string get_autoindex(void){return _autoindex;};
        std::string get_cgi_bin(void){return _cgi_bin;};
        std::string get_upload(void){return _upload;};
        int get_nb(void){return _nb;};
        long get_client_max_body_size(void){return _client_max_body_size;};
        std::vector<int>  get_ports(void){return _ports;};
        std::vector<std::string>  get_server_names(void){return _server_names;};
        int parse(void);
        bool check_location(std::map<std::string, std::vector<std::string> >  locations);
        void show_config();
        void show_index(void);
        void show_autoindex(void);
        void show_ports(void);
        void show_cgi(void);
        void show_cgi_bin(void);
        void show_error_pages(void);
        void show_roots(void);
        void show_server_names(void);
        void show_limit_except(void);
        void show_locations(void);
        void show_client_max_body(void);
        void show_upload(void);
        void show_all(void);
        int port(int p);
        bool exist_error_page(int code);
        
      

};
