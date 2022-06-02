#include "Request.hpp"

extern Logger log;

Request::Request() {
  //body_offset_ = 0;
  //chunk_size_ = 0;
  //_status = 0;
  _protocol = "HTTP/1.1";
  gettimeofday(&start_timer_, NULL);
}

Request::~Request() {}

int Request::parse(std::string &buffer) 
{
  size_t ret = 0;

  gettimeofday(&last_timer_, NULL);
  //std::cout << "nnn"  << std::endl;
  _buffer += buffer;
  buffer.clear();
  ret = method_line();
  ret = headers();
  ret = body();
  /*
  if (status_ == FIRST_LINE)
    ret = method_line();
  if (status_ == HEADERS)
    ret = headers();
  if (status_ == PREBODY)
    ret = prebody();
  if (status_ == BODY)
    ret = body();
  if (status_ == CHUNK)
    ret = chunk();
  if (statu,s_ == COMPLETE || ret == 1) {
    status_ = COMPLETE;
    return ret;
  }
  else if (status_ == ERROR || ret > 1) {
    status_ = ERROR;
    return ret;
  }
  */
  return ret;
}

//check path not going backwards
bool check_validity(std::string &uri) {
  int count = 0;

  std::string tmp = uri;

  while (tmp.find('/') != std::string::npos) {
    tmp = tmp.substr(tmp.find('/') + 1);
    if (tmp.empty())
      break;
    std::string tmp2 = tmp.substr(0, tmp.find('/'));

    if (tmp2.find("..", 0) != std::string::npos)
      count--;
    else
      count++;
  }
  return count >= 0;
}

int Request::method_line() {
  //std::cout << "nnn:  " << _buffer.substr(0, _buffer.find("\r\n")) << std::endl;
  std::string tmp;

  tmp = _buffer.substr(0, _buffer.find("\r\n"));
  // quitar espacios de mas
  // check length
  std::cout <<  "nb tokens:" << nb_tokens(tmp," ") << "  ."<< std::endl;
  if (nb_tokens(tmp," ") != 3)
  {
    log.print(INFO,"Invalid tokens",RED,true);
    return 400;
  }
  
  std::string method = get_token(tmp, " ",0);
  std::cout << "1 token: " << get_token(tmp, " ",0) << "  ."<< std::endl;
  std::string target = get_token(tmp, " ",1);
  std::cout << "2 token: " << get_token(tmp, " ",1) << "  ."<< std::endl;
  std::string protocol = get_token(tmp, " ",2);
  std::cout << "3 token: " << get_token(tmp, " ",2) << "  ."<< std::endl;
  
  //check method
  if (method != "GET" && method != "POST" && method != "DELETE")
  {
    log.print(INFO,"Invalid method",RED,true);
    return 501;
  }
  else
  {
    log.print(INFO,"Valid method",GREEN,true);
    _method = method;
  }
  
  //check path
  if (target[0]!= '/')
    return 400;
  if (target.length() > 100000) 
    return 414;
  if (!check_validity(target))
    return 403;
  
  log.print(INFO,"Valid target",GREEN,true);
  _target = target;

  //check protocol
  if (protocol != "HTTP/1.1")
  {
    log.print(INFO,"Invalid Protocol",RED,true);
    return 505;
  }
  else
  {
    log.print(INFO,"Valid Protocol",GREEN,true);
    _protocol = protocol;
  }
  //delete la primera linea
  size_t end = _buffer.find("\r\n");
  _buffer.erase(0, end + 2);
  return 0;
}

//Request-Headers los reonocidos Entity-Headers los no reconocidos
int Request::headers()
{
  std::string tmp;
  while (1)
  {
    tmp = _buffer.substr(0, _buffer.find("\r\n"));
    if (nb_tokens(tmp," ") == 0)
    {
      // graba el headers.
      break;
    }
        // graba el headers.
    std::string header;
    std::string header_value;

    header = get_token(tmp, ":", 0);   
    header_value = get_token(tmp, ":", 1);   

    std::cout << "tokens ------> "<< header << header_value << std::endl;
    //guardo los headers compruebo que existe antes?
    _request_headers[header]= header_value;
   //delete la primera linea
  size_t end = _buffer.find("\r\n");
  _buffer.erase(0, end + 2);
  }
  std::cout << "new request buffer" << std::endl;
  std::cout << "---------------" << std::endl;
  std::cout << _buffer << std::endl;
  std::cout << "---------------" << std::endl;
  return 0;
}
int Request::prebody()
{
  return 0;
}
int Request::body()
{
  std::string tmp;
  while (1)
  {
    tmp = _buffer.substr(0, _buffer.find("\r\n"));
    //if (tmp=="")
    //  break;
    if (nb_tokens(tmp," ") == 0)
    {
      break;
    }
    //delete la primera linea
    size_t end = _buffer.find("\r\n");
    _buffer.erase(0, end + 2);
  }
  std::cout << "new request body" << std::endl;
  std::cout << "---------------" << std::endl;
  std::cout << _buffer << std::endl;
  std::cout << "---------------" << std::endl;
  return 0;
}

time_t Request::get_start_timer_in_sec() {
  return start_timer_.tv_sec;
}

time_t Request::get_last_timer_in_sec() {
  return last_timer_.tv_sec;
}

bool Request::timeout() {
  //if (status_ != COMPLETE) {
  //  status_ = ERROR;
  //  return true;
  //}
  return false;
}

void Request::get_headers()
{
  std::cout << "Headers contains:\n";
  for (std::map<std::string, std::string>::iterator it=_request_headers.begin(); it!=_request_headers.end(); ++it)
    std::cout << it->first << " => " << it->second << '\n';
  return;
}