#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <stdio.h>      
#include <stdlib.h> 
#include <cstring>  
#include <sstream>
#include <string> 
#include <iostream>
#include "utils.hpp"
#include <sys/time.h>
#include "logger.hpp"

//check first line method (pos 1) and http version (pos last)
class Request {
 public:
  //friend class RequestConfig;

  Request();
  ~Request();

  void get_headers();
  std::string get_method(){return _method;}
  std::string get_protocol(){return _protocol;}
  std::string get_target(){return _target;}

  int parse(std::string &buffer);
  //int parse(char buffer[], int nbytes);
  
  int method_line();
  int headers();
  int prebody();
  int body();

  bool timeout();
  time_t get_start_timer_in_sec();
  time_t get_last_timer_in_sec();
  /*
  int chunk();
  int chunk_trailer();

  bool timeout();
  time_t get_start_timer_in_sec();
  time_t get_last_timer_in_sec();

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
 private:
  std::string _buffer;
  std::string _method;
  std::string _protocol;
  std::string _target;
  std::map<std::string, std::string> _request_headers;
  std::map<std::string, std::string> _request_entity;
  struct timeval start_timer_;
  struct timeval last_timer_;
  /*  
  std::string method_;
  std::string target_;
  std::string query_string_;
  std::string protocol_;
  std::string req_body_;
  std::map<std::string, std::string, ft::comp> headers_;

  int body_offset_;
  int chunk_size_;
  struct timeval start_timer_;
  struct timeval last_timer_;
  size_t length_;

  Status status_;
  ChunkStatus chunk_status_;
  */
};

#endif