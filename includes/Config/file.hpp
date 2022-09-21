#ifndef FILE_HPP
# define FILE_HPP

# include <iostream>

# include <fcntl.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <dirent.h>
# include <time.h>
# include <string.h>
# include <errno.h>
#include <iostream>
#include <cstdio>    // fopen, fclose, fread, fwrite, BUFSIZ
#include <ctime>
#include <cstdlib>
#include <iostream>
#include<fstream>
#include<sstream>
#include <fstream>
//#include <filesystem>

# include <algorithm>
# include <vector>

//# include "MimeTypes.hpp"
# include "logger.hpp"
//# include "StringUtils.hpp"
//# include "MimeTypes.hpp"

extern Logger log;
//extern MimeTypes g_mimes;

class File {
 public:
  File();
  File(std::string path);
  ~File();

  bool open(bool create = false);
  void close(void);
  void unlink(void);
  void create(std::string &body);
  void append(std::string &body);
  void set_path(std::string path, bool negociation = false);
  bool is_directory();
  std::string last_modified();
  bool exists(void);
  bool exists(std::string &path);

  std::string find_index(std::vector<std::string> &indexes);
  void parse_match();

  std::string autoIndex(std::string &target);
  std::string &getMimeExtension();
  void parseExtensions();
  void parseExtensionsNegociation();
  std::string getContent();
  //getters
  std::string &getPath();
  std::vector<std::string> &getMatches();
  int &getFd();

 private:
  int _fd;
  std::string _file_name;
  std::string _file_name_full;
  std::string _path;
};

#endif
