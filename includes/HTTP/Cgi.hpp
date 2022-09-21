#ifndef CGI_HPP
# define CGI_HPP

# include <iostream>
# include <map>

# include <sys/types.h>
# include <sys/wait.h>
#include <stdlib.h>
#include "config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "file.hpp"
#include "cstring"
#include "algorithm"

class CGI {
 public:
    CGI(Request &request);
    ~CGI();
    void init(int worker_id);
    int execute();
    bool setCGIEnv();
    std::string &getBody();
 private:
  Request _request;
  std::string _body;
  std::string _file;
  File _tmp_file;
  Config _config;
  std::string _current_directory;
  std::string _extension;
  std::map<std::string, std::string> _cgi_env;
  char **_env;
  char *_argv[3];
};
#endif
