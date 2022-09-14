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
class Parse_options
{
    private:
        int             _argc;
        char            **_argv;
        std::string     _path;
        bool            _help;
        bool            _uri;
        int             _level;
        bool            _test;
    public:
        Parse_options(int argc, char **argv);
        ~Parse_options();

        bool parse(void);
        std::string get_path(){return _path;};
        bool help();
        bool test();
        bool location();
        std::string helpText(void);
        bool get_test(void){return _test;};
};
