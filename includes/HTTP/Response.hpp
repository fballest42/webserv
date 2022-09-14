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
#include "config.hpp"
#include "Request.hpp"
#include "file.hpp"
#include "Cgi.hpp"

struct auto_listing {
  std::string name;
  bool is_dir;
  std::string date;
  size_t size;
};
extern pthread_mutex_t g_write;

class Response 
{
    private:
        std::string _version;
        int         _contentLength;
        //std::string _charset;
        //std::string _location;
        std::string _date;
        Request     _request;
        int         _stat;                      //status from request error 
        File        _file;                         
        std::string _response;                  //salida final = header+body
        std::string _header;                    //header for response
        std::string _body;                      //body for response
        int         _type;
        bool        _is_a_cgi;
        std::map<std::string, std::string> _response_headers;
    public:
        Response(Request newrequest, int _status);
        ~Response();
        void initResponse();
        int createErrorPage(int code);
        std::string autoindex(std::string &root, std::string &path);
        void build_body();
        int method_Delete();
        int method_Post();
        int method_Get();
        void setHeader();
        int sendResponse(int socket);
        bool is_allow_method(std::string met);
        bool get_is_a_CGI(){return _is_a_cgi;};
        int get_stat(){return _stat;};
};
#endif