#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <stdio.h>      
#include <stdlib.h> 
#include <cstring>  
#include <sstream>
#include <string> 
#include <iostream>
#include <sstream>
#include "utils.hpp"
#include <sys/time.h>
#include "logger.hpp"
#include "config.hpp"

//check first line method (pos 1) and http version (pos last)
class Request {

  private:
    std::string _buffer;
    std::string _protocol;
    
    std::map<std::string, std::string> _request_entity;
    std::string _body;
    int         _content_length;
    int         _body_offset;
    bool        _is_chunked;
    struct timeval _start_timer;
    struct timeval _last_timer;
    std::string _target;
    std::string _query_string;
    std::string _fileName;
    //std::string _pathCGI;
    std::string _method;
    bool        _isFile;
    std::string _pathInfo;
    std::string _pathAfter;
    std::string _actualLocation;


  public:
    Config      _config;
    std::map<std::string, std::string> _request_headers;

    Request(Config conf);
    ~Request();

    int parse(std::string &buffer);  
    int method_line();
    int headers();
    int chunked();
    int body();
    int join_Chunks();

    std::string get_method(){return _method;}
    std::string get_protocol(){return _protocol;}
    std::string get_target(){return _target;}
    std::string get_fileName(){return _fileName;}
    //std::string get_pathCGI(){return _pathCGI;}
    std::string get_query(){return _query_string;}
    std::string get_pathInfo(){return _pathInfo;}
    std::string get_pathAfter(){return _pathAfter;}
    std::string get_actualLocation(){return _actualLocation;}
    std::map<std::string, std::string> get_map_headers(){return _request_headers;} 
    bool get_isFile(){return _isFile;}
    bool get_isChunked(){return _is_chunked;}

    void get_headers();
    std::string get_body(){return _body;}
    bool timeout();
    time_t get_start_timer_in_sec();
    time_t get_last_timer_in_sec();
    void show_request_data();
  /*
  int chunk();
  int chunk_trailer();

  int getStatus();

  enum Status {
    FIRST_LINE,
    HEADERS,
    PREBODY,
    BODY,
    CHUNK,
    COMPLETE,
    ERROR
  };

  enum ChunkStatus {
    CHUNK_BODY,
    CHUNK_SIZE,
  };
  */
};

#endif