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



//Calse para guardar la configuración de un servidor
class WebServer_Exception : public std::exception
{
    public:

        const char * info;

        WebServer_Exception(const char*msg): info(msg) {}
        const char * what() const throw()
        {
            return (info);
        }

        const char* get_info() const {return info;};

};
