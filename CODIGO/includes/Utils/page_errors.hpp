#pragma once
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

class Error_page
{
    private:
        std::map<int ,std::string>          _error_pages;
    public:
        Error_page();
        ~Error_page();
        std::string get_error_page(int error_type);
        std::string get_error_description(int error_type);
};

