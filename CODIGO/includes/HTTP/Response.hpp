#ifndef RESPONSE_HPP
#define RESPONSE_HPP

//https://github.com/sqatim/WebServer_42/blob/main/Server/headers/Response.class.hpp
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include "page_errors.hpp"


class Response 
{
    private:
        std::string _version;
        std::string _status;
        std::string _contentType;
        int _contentLength;
        std::string _charset;
        std::string _location;
  
        std::string _response;                  //salida final
        std::string _header;
        std::string _body;
        //std::vector<std::string>    _setCookies;
        int _type;
    public:
        Response();
        ~Response();
        void initResponse();
        void setHeader();
        void setResponse();
        int sendResponse(int socket);
};
#endif