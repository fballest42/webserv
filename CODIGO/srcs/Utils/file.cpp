#include "file.hpp"

File::File() : _fd(0) {}

File::File(std::string path) : _fd(0) {
  set_path(path);
}

File::~File() {
  close();
}

void File::set_path(std::string path, bool negociation) {
  _path = path;
}

bool File::open(bool create) {
  close();

  if (create)
    _fd = ::open(_path.c_str(), O_CREAT | O_RDWR | O_TRUNC, 00755);
  else
    _fd = ::open(_path.c_str(), O_RDONLY);
  //log.print(DEBUG, "create fd from file: " + std::to_string(_fd) + " of " + _path, GREEN, true);

  return _fd > 0;
}

void File::close() {
  if (_fd <= 0)
    return ;

  ::close(_fd);
  _fd = 0;
}

void File::unlink(){
  if (!exists())
    return;
  if (::unlink(_path.c_str()) == -1)
    log.print(DEBUG, "unlink: " + std::string(strerror(errno)) + " of " + _path, RED, true);
}

void File::create(std::string &body) {
  if (!open(true)){
    log.print(INFO,"Error create:"+ std::string(strerror(errno)) + "of" + _path ,RED,true); 
    return;
  }
  if (body.length() && write(_fd, body.c_str(), body.length()) <= 0){
     log.print(INFO,"Error create:"+ std::string(strerror(errno)) + "of" + _path ,RED,true); 
  }
  log.print(INFO,"File created",GREEN,true); 
}

void File::append(std::string &body) {
  close();
  _fd = ::open(_path.c_str(), O_RDWR | O_APPEND);
  if (_fd < 0)
    return;
  if (body.length() && write(_fd, body.c_str(), body.length()) <= 0){
     log.print(INFO,"Error append:"+ std::string(strerror(errno)) + "of" + _path ,RED,true); 
  }
}

//Check if the file exits 
bool File::exists(){
  _fd = ::open(_path.c_str(), O_RDONLY);
  return(_fd > 0);
}

//Check if the file exits in the path
bool File::exists(std::string &path){
  _fd = ::open(path.c_str(), O_RDONLY);
  return(_fd > 0);
}

//create a string from the file in the path 
std::string File::getContent(){
  std::string final;
  char buf[4096+1];
  int ret;

  lseek(_fd,0,SEEK_SET);
  while((ret = read(_fd, buf, 4096))!=0)
  {
      if (ret == -1) 
      {
        //log.print(DEBUG, "read: " +std::string(strerror(errno)), RED,true);
        return "";
      }
      buf[ret] = '\0';
      final.insert(final.length(),buf, ret);
  }
  //std::cout << final << std::endl;
  return final;
}

//return path string
std::string &File::getPath(){
  return _path;
}

//return file descriptir fd of file
int &File::getFd(){
  return _fd;
};

std::string File::last_modified() {
  struct stat statbuf;
  struct tm	*tm;
  char buf[32];

  stat(_path.c_str(), &statbuf);
  tm = gmtime(&statbuf.st_mtime);
  int ret = strftime(buf, 32, "%a, %d %b %Y %T GMT", tm);
  return std::string(buf, ret);
}