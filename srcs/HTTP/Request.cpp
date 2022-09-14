/*
The shape of a request

Basically, all requests look like this:

[METH] [REQUEST-URI] HTTP/[VER]
[fieldname1]: [field-value1]
[fieldname2]: [field-value2]

[request body, if any]

User-Agent :This is a string identifying the user agent. An English version of Netscape 4.03 running under Windows NT would send "Mozilla/4.03 [en] (WinNT; I ;Nav)". (Mozilla is the old name for Netscape. See the references for more details.)
Referer :The referer field (yes, it's misspelled in the standard) tells the server where the user came from, which is very useful for logging and keeping track of who links to ones pages.
*/

#include "Request.hpp"

extern Logger log;

//Constructor
Request::Request(Config conf): _config(conf)
{
  //body_offset_ = 0;
  //chunk_size_ = 0;
  //_status = 0;
  _protocol = "";
  _query_string = "";
  _isFile = false;
  _fileName = "";
  _is_chunked = false;
  _content_length = 0;
  gettimeofday(&_start_timer, NULL); //get time and store in start_timer
}

//Destructor
Request::~Request() {}

//Parsing Request
int Request::parse(std::string &buffer) 
{
  size_t ret = 0;
  gettimeofday(&_last_timer, NULL);
  _buffer += buffer;
  buffer.clear();
  
  if ((ret = method_line()) > 0)
    return ret;
  //log.print(INFO,"METHOD LINE OK",RED, true);

  //Check prebody for chunked data
  //std::cout << "BUFFER check:" << _buffer << std::endl;
  if ((ret = headers()) > 0)
    return ret;
  //log.print(INFO,"HEADERS OK",RED, true);
  
  //check there is a content-lenght and check is < max_body_size
  if (_request_headers.find("content-length") != _request_headers.end())
  {
    if (stol(_request_headers["content-length"]) > _config.get_client_max_body_size() )
      return 413;
  }
  if ((ret = chunked()) > 0)
    return ret;
  if (!get_isChunked()) // is not chunked ????
  {
    if ((ret = body()) > 0 )
      return ret;
  }

  if (get_isChunked() && (ret = join_Chunks())) // is chunked ????
    return ret;

  //Check body size
  if (_body.length() > _config.get_client_max_body_size())
    {
      //log.print(INFO,"Invalid Body Size",RED,true);
      return 413;
    }
  //if (_body.empty() == true && _method == "POST")
  //    ret = 405;
  return ret;
}

/*
check path not going backwards '..'
*/
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

/* Read method line and do some previous checks:
  - Check Protocol: HTTP/1.1            ----> error 505
  - Allowed methods GET, POST, DELETE   ----> error 501
  - Check target: 
    - begin with '/'                    ----> error 400
    - lenght > 100000                   ----> error 414
    - no back '..'                      ----> error 403
    - is in locations                   ----> error 404,
  - Check is File request
  - Check it has parameters ?x=10.
*/
int Request::method_line() {
  std::string tmp;
  int ret = 0;
  //take first line
  tmp = _buffer.substr(0, _buffer.find("\r\n"));
  // check number of tokens = 3
  if (nb_tokens(tmp,' ') != 3)
    return 400;
  // Get tokens with split
  std::string method = get_token(tmp, ' ',0);
  std::string target = get_token(tmp, ' ',1);
  _target = target;
  std::string protocol = get_token(tmp, ' ',2);
  
  //log.print(INFO,"server : >>  [method: " + method + "] [target: " + target + "] [protocolo: " + protocol + "] [server: " + std::to_string(_config._nb) + "]",YELLOW,true);
  ///------------------ CHECK METHOD ----------------------------/////
  if (method != "GET" && method != "POST" && method != "DELETE")
    return 501;
  else
    _method = method;
  ///------------------ CHECK PROTOCOL ---------------------------/////
  if (protocol != "HTTP/1.1")
    return 505;
  else
    _protocol = protocol;
  ///------------------ CHECK TARGET -----------------------------/////
  //check target begin with slash
  if (target[0]!= '/')
    return 400;
  //Check length of the path
  if (target.length() > 100000) 
    return 414;
  //Check no backwards ".." path
  if (!check_validity(target))
    return 403;
  //Check it has parameters something after 
  if (target.find('?') != std::string::npos)
  {
    _query_string = target.substr(target.find('?')+1);
    target= target.erase(target.find('?'));
  }

  //Check the path is in locations or not.-->error 404   
  /*   check if target begin with a location  */
  int l =0;
  _actualLocation = ""; //init actual location
  _pathInfo = target; //init  _pathInfo
  std::map<std::string, std::vector<std::string> >::reverse_iterator rit;
  for (rit=_config._locations.rbegin(); rit!=_config._locations.rend(); ++rit)
  {
    //log.print(INFO,"comparador:" + rit->first + "----" + target ,GREEN,true);
    // std::string loc1 = rit->first;
    // if (loc1.size()>1 && loc1[loc1.size()-1]=='/')
    //   loc1.pop_back();
    // l = loc1.length();

    l = rit->first.length();
    if (target.substr(0,l).compare(rit->first) == 0)
    {
       log.print(INFO,"done:" + rit->first +" "+ target, GREEN,true);
       _actualLocation = rit->first;
       _pathInfo = target.substr(l,target.length());
       break;
    }
  }

  //if (target != "/")
  //{
    if (rit  == _config._locations.rend()) // NOT in locations
    {  
      //log.print(INFO,"NOT target in locations:" + target ,RED,true);
      std::cout << _config._locations.size() << std::endl;
      ret = 404;
    }
    else  // IS in location, separe LOCATION and PATHINFO
    { //////////////////////// redirection /////////////////////////////////////////////
      log.print(INFO,"INTERNAL REDIRECT TO : [location: " + rit->first + "]", YELLOW,true);
      Config new_config;
      //Have to change server config with  config
      new_config.set_Config(_config._locations.at(rit->first), _config._nb);
      new_config.parse();
      
      // !!!!!!!!! no hereda de atras y dejar default / 
      //log.print(INFO,"NEW CONFIG", YELLOW,true);
      
      //CHANGE CONFIG FILE WITH NEW CONFIGURATION

      //root  ------------ SI NO HAY ROOT EN LA NUEVA CONFIGURACIÓN USA LA ANTERIOR -------
      if (new_config._root!="")
          _config._root = new_config._root;
      //server names
      //new_config._server_names = _config._server_names;
      //index  -------- 
      if (!new_config._index.empty())
          _config._index = new_config._index;
      //autoindex
      _config._autoindex = new_config._autoindex;
      //upload
      _config._upload = new_config._upload;
      //clien max body size
      _config._client_max_body_size = new_config._client_max_body_size;
      //limits except
      _config._limits_except = new_config._limits_except;
      //CGI-BIN
      if (!new_config._cgi_bin.empty())
      {
        // std::cout << "ESTOY AQUI -------------------------------------------------\n"; 
        // std::cout << _config._cgi_bin << std::endl;
        // std::cout << new_config._cgi_bin << std::endl;
        _config._cgi_bin = new_config._cgi_bin;
      }
      //CGI
      _config._cgi = new_config._cgi; 
      _config._locations.clear();
      //_config.show_all();
      //ERROR_PAGES
      if (!new_config._error_pages.empty())
      { 
        _config._error_pages.clear();
        _config._error_pages = new_config._error_pages;
        log.print(INFO,"NEW ERROR PAGES", YELLOW,true);
      }
    }
  //}

  ///// localizar file y path before and after file.....

  int tokens = nb_tokens(target,'/');
  std::string tmp_path;
  std::string file;

  //CHECK CGI HTTP:://127.0.0.1/cgi-bin/hello.pl
  //http://www.SOMEWHERE.org/cgi-bin/ice_cream.plx?flavor=mint
  if (nb_tokens(target,'.') > 1)  // there is a file
  {
    //log.print(INFO,"THERE IS a FILE" + target + _actualLocation, RED,true);
    //DELETE LOCATION
    std::size_t pos = 0;
    pos = target.find(_actualLocation);
    target= target.substr(pos+_actualLocation.length(), target.length());
    //log.print(INFO,"THERE IS a FILE" + target + _actualLocation, RED,true);

    if (nb_tokens(target,'.')>2)  //CHECK IF MORE THAN ONE ....
    {
      //log.print(INFO,"MORE THAN ONE  '.' ",RED,true);
      return 400;
    }
    int i = 1;
    while (i < tokens){
        if (nb_tokens(get_token(target,'/', i),'.') > 1)
        break;
        tmp_path += get_token(target,'/', i) + "/";
        i++; 
    }   
    //DELETE LOCATION FROM PATH INFO
    if (tmp_path[tmp_path.size()-1]=='/')
      tmp_path.pop_back();
    _pathInfo = tmp_path;

    //FILE
    file = get_token(target,'/', tokens-1);
    _isFile = true;
    _fileName = get_token(target,'/', i);
    //PATH AFTER FILE
    pos = target.find(_fileName);
    _pathAfter = target.substr(pos +_fileName.length(), target.length());
    if (_pathAfter[0]=='/')
      _pathAfter = _pathAfter.substr(1,_pathAfter.length());
    log.print(INFO,"server : >>  [File: " + _fileName  + "] [Path: " + _pathInfo + "] [PathAfter: " + _pathInfo  + "] [Parameters: " + _query_string + "] [server: " + std::to_string(_config._nb) + "]",YELLOW,true);
    ret = 0;
  }
  else // there is not  file
  {
    tmp_path= "";
    std::size_t pos = 0;
    pos = target.find(_actualLocation);
    //log.print(INFO,"--------------]" + std::to_string(pos) + target + "---" + _actualLocation,YELLOW,true);
    tmp_path = target.substr(pos + _actualLocation.length(), target.length());
    if (tmp_path[0]=='/')
      tmp_path = tmp_path.substr(1,tmp_path.size());
    if (tmp_path[tmp_path.size()-1]=='/')
      tmp_path = tmp_path.substr(0,tmp_path.size()-1);
    _pathInfo = tmp_path;
    _isFile= false;
    ret = 0;  
  }

  //Check is target is in location and put a '/' at the end is not exits
  if (target[target.length()-1]!='/')
    target = target + '/';
 
  //delete first line of the buffer
  size_t end = _buffer.find("\r\n");
  _buffer.erase(0, end + 2);

  return ret;
}

/* Read Headers, Request-Headers los reconocidos y Entity-Headers los no reconocidos
  - Check name header length > 1000  -->error 400
  - Check value header length > 4000 -->error 400 
  - If no Host header or blank -------->error 400
  - header "Host" don´t have "@" if yes ->error 400
  - header "Content-Length" is exits, is a positive number ---->400 
*/
int Request::headers()
{
  std::string tmp;
  while (1)
  {
    int c;
    tmp = _buffer.substr(0, _buffer.find("\r\n"));
    c = nb_tokens(tmp,':');
    //std::cout << "NNNN--NB-" << std::to_string(c) << std::endl;
    if (c == 0 ) 
    {
      break;
    }
    //if (c != 2)
    //  return 400;
    
    // graba el headers.
    std::string header;
    std::string header_value;
   
    std::string tmp1;
    tmp1 = get_token(tmp, ':', 0);
    if (tmp1[tmp1.length()-1] == ' ')
      return 400;

    header = ft_clean_token(get_token(tmp, ':', 0));  

    if (c < 2 )
      return (400); 
    else
      header_value = ft_clean_token(get_token(tmp, ':', 1));
    // Check length of header (<=1000) and header (value <=4000)
    if (header.length() > 1000 || header_value.length() > 4000)
      return 400;
    // Check if only one "Host" header.
    if (header == "host" || header == "content-length")
    {
      if (header == "host")
      {
        if (nb_tokens(header_value,',')>1) //to many server
          return 400;
        if (_request_headers.find("host") == _request_headers.end())
        {
          _request_headers[header]= header_value;
        }
        else 
          return 400;
      }
      if (header == "content-length")
      {
        if (_request_headers.find("content-length") == _request_headers.end())
          _request_headers[header]= header_value;
        else 
          return 400;
      }
    }
    else 
    {
      _request_headers[header]= header_value;
    }

    // Check if not headers
    if (header.empty())
      return 400;

    // CHECK IF HOST IS CORRECT
    // for (std::vector<std::string>::iterator it = this->_config._server_names.begin() ; it != this->_config._server_names.end(); ++it)
    // {    if (*it == _request_headers["host"]){
    //         break;
    //     };
    // if (it != this->_config._server_names.end())
    // {
    //   return 999;
    // }
    // }
    // std::cout << '\n';

    // if (this->_config.find(_request_headers["host"]))
    //   _request_headers.find("host"))
    //delete la primera linea
    size_t end = _buffer.find("\r\n");
    _buffer.erase(0, end + 2);
    //return 0;
  }

  //Checking there are headers
  // if (_request_headers.empty())
  //   return (400);

  //Checking some headers
  // if header "Host" exits and is not blank
  if (_request_headers.find("host") == _request_headers.end() || _request_headers["host"].empty())
    return 400;
  // header "Host" don´t have @
  if (_request_headers["host"].find("@") != std::string::npos)
    return 400;
  std::vector<std::string>::iterator it ;
  for (it = _config._server_names.begin(); it != _config._server_names.end(); ++it)
  {
    if (_request_headers["host"] == *it)
      break;
  }
  if (it == _config._server_names.end())
      return 400;
  // header "Content-Length" is exits, is a positive number a store in _content_length
  if (_request_headers.find("content-length") != _request_headers.end()) {
    if (_request_headers["content-length"].find_first_not_of("0123456789") != std::string::npos)
      return 400; //non numeric symbol or '-'
    try {
      _content_length = std::stol(_request_headers["content-length"]);
    }
    catch (std::exception &e){ 
        return 400;
    }
  }
  //get_headers();
  return 0;
}

/* Read preBody */
int Request::chunked()
{
  _body_offset = 0; 
  //std::cout << "--------------------------------------CHECKING IS A CHUNKED REQUEST:?\n" << std::endl;
  if (_request_headers.find("transfer-encoding") != _request_headers.end() && _request_headers["transfer-encoding"] == "chunked"){
    _is_chunked = true;
    //std::cout << "--------------------------------------IS A CHUNKED REQUEST:\n";
  }
  return 0;
}

/* Read Body */
int Request::body()
{
  //// quito la primera linea \r\n
  std::string tmp;
  // while (1){
    tmp = _buffer.substr(0, _buffer.find("\r\n"));
    std::cout << tmp << std::endl;
    if (tmp.empty()){
      size_t end = _buffer.find("\r\n");
      _buffer.erase(0, end + 2);
    }
  //   else {
  //     break;
  //   }
  // }
  
  // If length body < que defined header _content_length error 400.
  if (_buffer.length() < _content_length)
  {
    log.print(INFO,"_BUFFER LENGTH 1 " + std::to_string(_buffer.length()) + " " + std::to_string(_content_length) ,RED,true);
    return 400;
  }
  //rest of buffer save in _body
  if (_buffer.length() >= _content_length) {
    _body.insert(_body_offset, _buffer, 0 , _content_length);
    _body_offset += _buffer.length();
    _buffer.clear(); 
    if (_body.length() == _content_length)
    {
      // std::cout << "Body:"  << std::endl;
      // std::cout << "Body length :" << _buffer.length() << std::endl;
      //std::cout << "Content length :" << _content_length << std::endl;
      //std::cout << _body << std::endl;
      return 0;
    }
    else
    { 
      //log.print(INFO,"_BUFFER LENGTH 2 ",RED,true);
      return 400;
    }  
  }
  // std::cout << "Body:"  << std::endl;
  // std::cout << "Body length :" << _buffer.length() << std::endl;
  // std::cout << "Content length :" << _content_length << std::endl;
  //std::cout << _body << std::endl;
  return 0;
}

int Request::join_Chunks(){   ////// /r/n OJOJOOJOJOJOOJOJOJOOOJO Y SUBT
  // first line --> hex number with length of the data in the chunk
  // second line ---> data
  // ends chunks if 0
  // last lines? headers: value 
  std::cout << " CHUNKING :\n" << _buffer << std::endl;
  size_t end;
  size_t size;
  size_t size_line;
  end = _buffer.find("\r\n");
  _buffer.erase(0, end + 2); // borro la linea tratada.
  while ((end = _buffer.find("\r\n")) != std::string::npos)
  {
    //Size line
    //std::cout << " THE CHUNK LINE IS :" << _buffer.substr(0,end) << std::endl;
    std::string hex = _buffer.substr(0,end);
    std::cout << " THE CHUNK LINE IS :" << hex << std::endl;
    try {
      size = (int) strtol(hex.c_str(), NULL, 16);
      _buffer.erase(0, end + 2); // borro la linea tratada.
      std::cout << " THE SIZE CHUNK IS :" << size << std::endl;
    }
    catch (std::exception &e){ 
        return 400;
    }
    if (size == 0) {
      std::cout << " GOTO TO HEADERS:" << size << std::endl;
      //_buffer.erase(0, end + 1); // borro la linea tratada.
      break;     //go to headers---->
    }
    end = _buffer.find("\r\n");
    //Data line
    if (end != std::string::npos){    // Tiene que haber linea de datos
      std::cout << " THE DATA CHUNK IS :" <<  _buffer.substr(0, end) << " size: " << _buffer.substr(0, end).length() << std::endl;
      size_line = _buffer.substr(0, end).length();
      if (size_line != size)  // Check is size is correct
        return 400;
      _body += _buffer.substr(0, end); 
      _buffer.erase(0, end+2); // borro la linea tratada.
    }
    else {
      return 400;
    }    
  }
  //end = _buffer.find("\n");
  //_buffer.erase(0, end+1); // borro la linea tratada.
  //is any headers?
  std::string tmp;
  while ((end = _buffer.find("\r\n")) != std::string::npos) {
    std::cout << " Checking header :" << tmp << std::endl;
    if (_buffer.find("\r\n") == 0) { //no hay mas que ver
      //_buffer.erase(0, end + 2);
      return 226;
    }
    else {
      tmp = _buffer.substr(0, _buffer.find("\r\n"));
      std::cout << " THE line header CHUNK IS :" << tmp << std::endl;
      if (tmp.empty())
        return 226;
      if (nb_tokens(tmp,':') != 2)
      {
        if (nb_tokens(tmp,' ') != 1)
          return 400;
        return 226;
      }
      // graba el headers.
      std::string header;
      std::string header_value;

      header = get_token(tmp, ':', 0);  
      if (header[header.size()-1]==':') // Delete ":"
          header.pop_back(); 

      header_value = get_token(tmp, ':', 1);
      if (!header.empty())
        _request_headers[header]=header_value;   
      /*
      else
        return 400;
      */
      _buffer.erase(0, end + 2);
    }
  }

  std::cout << "BODY CHUNKED:"  << std::endl;
  std::cout << _body << std::endl;
  return 0;
}

time_t Request::get_start_timer_in_sec() {
  return _start_timer.tv_sec;
}

time_t Request::get_last_timer_in_sec() {
  return _last_timer.tv_sec;
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

void Request::show_request_data(){
  std::cout << "-- METHOD LINE" << std::endl;
  std::cout << _method << " " << _target << " " << _protocol << std::endl;
  std::cout << "-- HEADERS" << std::endl;
  for (std::map<std::string, std::string>::iterator it = _request_headers.begin(); it!=_request_headers.end(); ++it)
    std::cout << it->first << " => " << it->second << '\n';
  return;
}