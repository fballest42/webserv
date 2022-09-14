#include "Cgi.hpp"

extern Logger log;

CGI::CGI(Request &request): _request(request), _file(request.get_fileName()), _config(request._config){
  log.print(INFO,"server : >>  Init cgi",BLUE,true);
  //req_body_ = file_.getContent();
}

CGI::~CGI() {
  free(_argv[0]);
  free(_argv[1]);
  if (_env){
    int i = 0;
    while (_env[i]) {
      if (_env[i])
        free(_env[i]);
      _env[i] = NULL;
      i++;
    }
    free(_env);
  }
  _tmp_file.close();
  _tmp_file.unlink();
}

void CGI::init(int worker_id) {
  
  _extension = "." + get_token(_file, '.', 1); 
  _env = NULL;
  _argv[0] = NULL;
  _argv[1] = NULL;
  _argv[2] = NULL;
  //get current directory and store in a variable
  char *cwd = getcwd(NULL, 0); 
  if (!cwd)
    return ;
  _current_directory = cwd;  //store in variable
  free(cwd);

  //create a tmp file to store result
  std::string cgi_path = "/tmp/webserv_cgi_tmp_1";
  _tmp_file.set_path(cgi_path.c_str());
  _tmp_file.open(true);

  //log.print(DEBUG, "server : CGI -> " + this->_request._config.get_cgi_bin(),BLUE,true);
}
// Set the envionment variables 
bool CGI::setCGIEnv() {
  // pass information along from the request to enviorement variables
  for (std::map<std::string, std::string>::iterator it = _request._request_headers.begin(); it!=_request._request_headers.end(); ++it)
  {
    std::string str = it->first;
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    std::string tmp;
    strip(it->second, tmp);
    std::string value_tmp;
    value_tmp = tmp;
    std::transform(value_tmp.begin(), value_tmp.end(), value_tmp.begin(), ::toupper);
    std::string header = "HTTP_" + str;
    std::replace(header.begin(), header.end(), '-', '_');
    _cgi_env[header] = value_tmp;
    //std::cout << "/////////////////////////////////////" << it->second << "////" << std::endl;
  }

  //_cgi_env["COMSPEC"] = "CGI/1.1";
  _cgi_env["DOCUMENT_ROOT"] = "CGI/1.1";
  _cgi_env["GATEWAY_INTERFACE"] = "CGI/1.1";
  //_cgi_env["HOME"] = "CGI/1.1";

  //_cgi_env["PATH"] = _request.get_protocol();
  //_cgi_env["PATHEXT"] = _request.get_protocol();
  _cgi_env["PATH_INFO"] = _request.get_pathInfo();
	//_cgi_env["PATH_TRANSLATED"] = file_path_;
  _cgi_env["QUERY_STRING"] = _request.get_query();
  
  //_cgi_env["REMOTE_ADDR"] = _request.get_protocol(); //127.0.0.1
  //_cgi_env["REMOTE_PORT"] = _request.get_protocol(); //
  
  _cgi_env["REQUEST_METHOD"] = _request.get_method();   
	_cgi_env["REQUEST_URI"] = _request.get_pathInfo();              // /cgi-bin/pp.py/foo//bar/?x=1&y=1
  
  _cgi_env["SCRIPT_FILE"] = this->_current_directory + "/" + this->_request._config.get_root() + "/" + this->_request._config.get_cgi_bin() + "/" + this->_request.get_fileName();       //full path script
  _cgi_env["SCRIPT_NAME"] = "/" + this->_request._config.get_cgi_bin() + "/" + this->_request.get_fileName();       // /cgi-bin/pp.py
	
  //_cgi_env["SERVER_ADDR"] = config_.getHost();
  _cgi_env["SERVER_ADMIN"] = "email@email.com";                 
  //_cgi_env["SERVER_NAME"] = config_.getHost();
	//_cgi_env["SERVER_PORT"] = ft::to_string(config_.getPort());
  _cgi_env["SERVER_PROTOCOL"] = _request.get_protocol();        
  _cgi_env["SERVER_SIGNATURE"] = "";                            
  _cgi_env["SERVER_SOFTWARE"] = "WEBSERV/1.0";                  

  _cgi_env["SYSTEMROOT"] = _request.get_protocol();
  _cgi_env["TERM"] = _request.get_protocol();

	if (_extension == ".php")
		_cgi_env["REDIRECT_STATUS"] = "200";
  
	if (!(_env = (char **)malloc(sizeof(char *) * (_cgi_env.size() + 1))))
		return false;

	int i = 0;

	for (std::map<std::string, std::string>::iterator it = _cgi_env.begin(); it != _cgi_env.end(); it++) {
		std::string tmp = it->first + "=" + it->second;
		if (!(_env[i] = strdup(tmp.c_str())))
      return false;
		i++;
	}
	_env[i] = NULL;
  // std::cout << "-- CGI _ ENV ----" << std::endl;
  // for (std::map<std::string, std::string>::iterator it = _cgi_env.begin(); it!=_cgi_env.end(); ++it)
  //   std::cout << it->first << " => " << it->second << '\n';
  return true;
}
/*
CGI::CGI(File &file, RequestConfig &config, std::map<std::string, std::string, ft::comp> &req_headers, std::string &req_body) : file_(file), config_(config), req_headers_(req_headers) {
  
  if (req_body.empty() && config_.getMethod() != "POST")
    req_body_ = file_.getContent();
  else
    req_body_ = req_body;
}

void CGI::init(int worker_id) {
  //get current directory
  char *cwd = getcwd(NULL, 0); 
  if (!cwd)
    return ;
  _current_directory = cwd;  //store in variable
  free(cwd);

  env_ = NULL;
  argv_[0] = NULL;
  argv_[1] = NULL;
  argv_[2] = NULL;

  extension_ = file_.getMimeExtension();

  cgi_exe_ = config_.getCGI()[extension_];
  if (config_.getCGIBin()[0] == '/') {
    cgi_path_ = config_.getCGIBin() + "/" + cgi_exe_;
  } else {
    cgi_path_ = _cwd + "/" + config_.getCGIBin() + "/" + cgi_exe_;
  }
  std::string cgi_path = "/tmp/webserv_cgi_tmp_" + ft::to_string(worker_id);
  tmp_file_.set_path(cgi_path.c_str());
  tmp_file_.open(true);
  if (worker_id)
    Log.print(DEBUG, "worker[" + ft::to_string(worker_id) + "] : CGI -> " + cgi_path_);
  else
    Log.print(DEBUG, "server : CGI -> " + cgi_path_);
}

*/
int CGI::execute() {
  // Build absolute path to access CGI file 
  // std::string file_p = this->_current_directory + "/" + this->_request._config.get_root() + "/" + this->_request._config.get_cgi_bin() + "/";
  std::string file_p = this->_current_directory + "/" + this->_request._config.get_cgi_bin() + "/";
  //log.print(INFO,"server : >>  Complete path of cgi file " + this->_request.get_fileName() ,BLUE,true);
  log.print(INFO,"[CGI file detected:" + file_p + this->_request.get_fileName()+"]",BLUE,true);
  
  // Set Enviroment
  if (!setCGIEnv())
    return 500;
  
  // Set arguments for execve
  if (!(_argv[0] = strdup(this->_request.get_fileName().c_str()))) // p.e "py_program.py"
    return 500;
  if (!(_argv[1] = strdup(this->_request.get_query().c_str()))) // p.e "key=value"
    return 500;
  _argv[2] = NULL;

  // Build a process to execute execve and redirect output to temporary file
  int pip[2];

  if (pipe(pip) != 0)
    return 500;

  pid_t pid = fork();

  // if (pipe(pip) != 0)
  //   return 500;
  if (pid == 0) {
    if (chdir(file_p.c_str()) == -1)
      return 500;
    close(pip[1]);
    if (dup2(pip[0], 0) == -1)
      return 500;
    if (dup2(_tmp_file.getFd(), 1) == -1)
      return 500;
    close(pip[0]);
    execve(_argv[0], _argv, _env);
    exit(1);
  }
  else if (pid > 0) {
    close(pip[0]);
    //if (req_body_.length() && write(pip[1], req_body_.c_str(), req_body_.length()) <= 0)
    //  return 500;
    close(pip[1]);
    int status;
    if (waitpid(pid, &status, 0) == -1)
      return 500;
    if (WIFEXITED(status) && WEXITSTATUS(status))
      return 502;
  }
  else {
    return 502;
  }
  _body = _request.get_body();
  _body += _tmp_file.getContent();
  return 226;
}

std::string &CGI::getBody() {
  return _body;
}
/*
void CGI::parseHeaders(std::map<std::string, std::string> &headers) {
  size_t end, last;
  std::string header;

  while ((end = body_.find("\r\n")) != std::string::npos) {
    if (body_.find("\r\n") == 0) {
      body_.erase(0, end + 2);
      break;
    }
    if ((last = body_.find(':', 0)) != std::string::npos) {
      header = body_.substr(0, last);
      headers[header] = ft::trim_left(body_.substr(last + 1, end - last - 1), ' ');
    }
    body_.erase(0, end + 2);
  }
  if (headers.count("Content-Length")) {
    size_t size = ft::stoi(headers["Content-Length"]);

    body_.erase(size);
  }
}

std::string &CGI::getBody() {
  return body_;
}

bool CGI::setCGIEnv() {
  if (config_.getMethod() == "POST") {
		cgi_env_["CONTENT_TYPE"] = req_headers_["Content-Type"];
		cgi_env_["CONTENT_LENGTH"] = ft::to_string(req_body_.length());
	}
	cgi_env_["GATEWAY_INTERFACE"] = "CGI/1.1";
  cgi_env_["PATH_INFO"] = file_path_;
	cgi_env_["PATH_TRANSLATED"] = file_path_;
  cgi_env_["QUERY_STRING"] = config_.getQuery();
  cgi_env_["REMOTE_ADDR"] = config_.getClient().getAddr();

  if (config_.getAuth() != "off") {
    cgi_env_["AUTH_TYPE"] = "Basic";
    cgi_env_["REMOTE_IDENT"] = config_.getAuth().substr(0, config_.getAuth().find(':'));
    cgi_env_["REMOTE_USER"] = config_.getAuth().substr(0, config_.getAuth().find(':'));
  }

  cgi_env_["REQUEST_METHOD"] = config_.getMethod();
	cgi_env_["REQUEST_URI"] = file_path_;

  cgi_env_["SCRIPT_NAME"] = cgi_path_;
	cgi_env_["SERVER_NAME"] = config_.getHost();
	cgi_env_["SERVER_PROTOCOL"] = config_.getProtocol();
	cgi_env_["SERVER_PORT"] = ft::to_string(config_.getPort());
  cgi_env_["SERVER_SOFTWARE"] = "WEBSERV/1.0";

	if (extension_ == ".php")
		cgi_env_["REDIRECT_STATUS"] = "200";

  for (std::map<std::string, std::string, ft::comp>::iterator it = req_headers_.begin(); it != req_headers_.end(); it++) {
    if (!it->second.empty()) {
      std::string header = "HTTP_" + ft::to_upper(it->first);
      std::replace(header.begin(), header.end(), '-', '_');
      cgi_env_[header] = it->second;
    }
  }

	if (!(env_ = (char **)malloc(sizeof(char *) * (cgi_env_.size() + 1))))
		return false;

	int i = 0;

	for (std::map<std::string, std::string>::iterator it = cgi_env_.begin(); it != cgi_env_.end(); it++) {
		std::string tmp = it->first + "=" + it->second;
		if (!(env_[i] = ft::strdup(tmp.c_str())))
      return false;
		i++;
	}
	env_[i] = NULL;
  return true;
}
*/