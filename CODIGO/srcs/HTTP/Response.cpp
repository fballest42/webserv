#include "Response.hpp"
//https://github.com/sqatim/WebServer_42/blob/main/Server/Response.class.cpp

extern Error_page err_page;

std::string get_http_date() {
    struct timeval tv;
    char buf[32];
    gettimeofday(&tv, NULL);

    struct tm	*tm;
    tm = gmtime(&tv.tv_sec);
    int ret = strftime(buf, 32, "%a, %d %b %Y %T GMT", tm);
    return std::string(buf, ret);
}


bool isCGI_extension(std::string file, std::map<std::string, std::string>  _cgi)
{
    std::string extension;
    extension = "." + get_token(file, '.', 1);
    log.print(INFO,"cgi extension:" + extension, GREEN,true);
    
    std::map<std::string, std::string>::iterator  it;
    it = _cgi.find(extension);
    if (it != _cgi.end())
        return 1;
    return 0;  
}

Response::Response(Request newrequest, int status): _stat(status), _request(newrequest)
{
    //log.print(INFO,"TEST",RED,true);
    initResponse();
    log.print(INFO," REQUEST DATA:" ,YELLOW,true);
    std::cout << "request._method: " << this->_request.get_method() << std::endl;
    //std::cout << "request._pathCGI: " << this->_request.get_pathCGI() << std::endl;
    std::cout << "request._isFile: " << std::to_string(_request.get_isFile()) << std::endl;
    std::cout << "request._fileCGI: " << this->_request.get_fileName() << std::endl;
    
    std::cout << "request._actualLocation: " << this->_request.get_actualLocation() << std::endl;
    std::cout << "request._pathInfo: " << this->_request.get_pathInfo() << std::endl;
    std::cout << "request._pathAfter: " << this->_request.get_pathAfter() << std::endl;
    std::cout << "request._query_string: " << this->_request.get_query() << std::endl;
    std::cout << "request._target: " << this->_request.get_target() << std::endl;
    this->_request.get_headers();
    log.print(INFO," ACTUAL CONFIG:" ,YELLOW,true);
    this->_request._config.show_all();
}

Response::~Response()
{
}

void Response::initResponse()
{
    
    _is_a_cgi = false;
    //this->_setCookies.clear();
    this->_version = "HTTP/1.1";
    this->_status = "200 OK";

    this->_contentType = "text/html";
    this->_contentLength = 0;
    this->_charset = "UTF-8";
    this->_location = "Location: ";
  
}

//Create a default error page or take the custom one
int Response::createErrorPage(int error_code){
    //check if custom page exist
    File tmp;
    if (_request._config.exist_error_page(error_code))
    {
        //tmp.set_path(_request._config.get_root() + _request._config.get_path_error_page(error_code));
        tmp.set_path(_request._config.get_path_error_page(error_code));
       
        //tmp.set_path("www/siteA/errors/404.html");
        // Check if no problem when open file
        if(tmp.open())
        {
            _body = tmp.getContent();   
            this->_contentLength = _body.length();
            _status = std::to_string(error_code) + " " + err_page.get_error_description(error_code);
            //return 0;
        }
        else //if problem put defautl
        {
            log.print(INFO,"Custom Page Error not found:"+ tmp.getPath() ,RED,true); 
            _status = std::to_string(error_code) + " " + err_page.get_error_description(error_code);
            _body = err_page.get_error_page(error_code);
            this->_contentLength = _body.length(); ///////////
        }
    }
    else     // create body with default error pages
    {
    _status = std::to_string(error_code) + " " + err_page.get_error_description(error_code);
    _body = err_page.get_error_page(error_code);
    this->_contentLength = _body.length();  ///////////
    }
    // create headers
    _response_headers["Content-Length"] = _body.length();
    _response_headers["Content-Type"] = ".html";
    if (error_code == 408 || error_code == 503 || error_code == 405)
        _response_headers["Connection"] = "close";
    if (error_code == 503)
        _response_headers["Retry-After"] = "30";
    return 0;
}

void Response::setHeader()
{
    /*
    //header = "HTTP/1.0 200 OK\r\n Content-Length: 11\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n"
    this->_header = this->_version;                 //HTTP/1.00  HTTP/1.0_
    this->_header += " " + this->_status + "\r\n";    //200-400... 200 OK\r\n
    this->_header += "Server: webserver42/1.0\r\n";

    //this->_header += "Access-Control-Allow-Origin: *\r\n";
    this->_header += "Connection: Keep-Alive\r\n";
    //this->_header += "Content-Encoding: gzip\r\n";
    this->_header += "Content-Length: " + std::to_string(this->_contentLength) + "\r\n";     //Content-Length: 11
    //headers
    this->_header += "Content-Type: " + this->_contentType + ";";       //Content-Type: text/html
    this->_header += "charset=" + this->_charset + "\r\n";              //charset=UTF-8\r\n\r\n
    this->_date = get_http_date(); 
    this->_header += "Date:" + this->_date + "\r\n";
    //this->_header += "Etag: \"jshdjshdjshdhsh\"\r\n";
    //this->_header += "Keep-Alive: timeout=5, max=997\r\n";
    //this->_header += "Last-Modified: Mon, 18 jul 2016 16:06:00 GMT\r\n";
    this->_header += "\r\n";
    */
    //Create header version 2
    std::cout << "--- HEADERS VERSION 2 ---" << std::endl;
    //std::cout << _stat << " " << err_page.get_error_description(_stat) << std::endl;
    _response_headers["Server"] = "webserver42/1.0";
    _response_headers["Content-Length"] = std::to_string(this->_contentLength);
    _response_headers["charset"] = this->_charset;
    this->_date = get_http_date();
    _response_headers["Date"] = this->_date;
    _response_headers["Connection"] = "close";
    // _response_headers["Content-Type"] = "text/html";
     //_response_headers["Connection"] = "Keep-Alive"; 
    //_response_headers["Keep-Alive"] = "timeout=5, max=997";

    // create header
    this->_header = this->_version;  
    this->_header += " ";
    this->_header += std::to_string(this->_stat); 
    this->_header += " ";
    std::cout << "AQUI====" << std::to_string(_stat) << std::endl;
    this->_header += err_page.get_error_description(_stat); 
    std::cout << "--- HEADERS VERSION 2B ---" << std::endl;
    this->_header += "\r\n"; 
    for (std::map<std::string, std::string>::iterator it=_response_headers.begin(); it!=_response_headers.end(); ++it)
    {
        std::cout << it->first << " => " << it->second << '\n';
        this->_header += it->first; 
        this->_header += + ": ";
        this->_header +=  it->second; 
        this->_header += "\r\n";
    }
    this->_header += "\r\n";

    return;


}

void Response::build_body()
{
    log.print(INFO,"--------------- START BUILD RESPONDE BUILD --------------", GREEN,true);
    log.print(INFO,"CHECK IF ERROR IN REQUEST: " + std::to_string(_stat), YELLOW,true);
    if (_stat >= 300)
    {
        log.print(INFO,"=> ERROR IN REQUEST: Creating error page:" + std::to_string(_stat),RED,true);
        createErrorPage(_stat);
        return;
    }
    log.print(INFO,"=> NOT ERROR IN REQUEST",YELLOW,true);
  
    //check if method is allowed
    log.print(INFO,"CHECK IF METHOD IS ALLOWED:" + _request.get_method(), YELLOW,true);
    if (!is_allow_method(_request.get_method()))
    {
        log.print(INFO,"=> METHOD NOT ALLOWED",RED,true);
        _stat = 501;
    }
    if (_stat >= 300)
    {
        log.print(INFO,"Creating error page:" + std::to_string(_stat),RED,true);
        createErrorPage(_stat);
        return;
    }
    log.print(INFO,"=> METHOD ALLOWED",YELLOW,true);

    //HANDLE METHOD
    //delete method
    log.print(INFO,"CHECK DELETE METHOD", YELLOW,true);
    if(_request.get_method()=="DELETE")
    {
        log.print(INFO,"IT IS DELETE METHOD", GREEN,true);
        if (!method_Delete())
            return;
    }
    if (_stat >= 300)
    {
        log.print(INFO,"Creating error page:" + std::to_string(_stat),RED,true);
        createErrorPage(_stat);
        return;
    }
    //post method
    log.print(INFO,"CHECK POST METHOD", YELLOW,true);
    if(_request.get_method()=="POST")
    {
        log.print(INFO,"IT IS POST METHOD", GREEN,true);
        if (!method_Post())
            return;
    }
    log.print(INFO,"CHECK GET METHOD", YELLOW,true);
    if(_request.get_method()=="GET")
    {
        log.print(INFO,"IT IS GET METHOD", GREEN,true);
        method_Get();
    }
    if (_stat >= 300 and !get_is_a_CGI())
    {
        log.print(INFO,"Creating error page:" + std::to_string(_stat),RED,true);
        createErrorPage(_stat);
        return;
    }
}

//return 0 if deleted ok, 1 if not. _stat=404 ********DONE
int Response::method_Delete(){
    //check if file exits? if not 404 -> _stat = 404
    int status;
    std::string path_file;
    path_file = "./" + _request._config.get_root()+"/"+ _request.get_pathInfo() + "/" +_request.get_fileName();
    status=remove(path_file.c_str());
    if (status==0)
        log.print(INFO,"File deleted",GREEN,true); 
    else 
    {
        log.print(INFO,"File Not found:" + path_file,RED,true); 
        _stat = 404;
        return 1;
    }
    _body += "<!DOCTYPE html>\n\
            <html>\n\
            <body>\n\
                <h1>File deleted.</h1>\n\
            </body>\n\
            </html>";
    this->_contentType = "text/html"; ///-----------------------------
    _response_headers["Content-Type"] = "text/html";
    return 0;
}

//return 0 if post ok, 1 if not. devuelve en el body el body que recibe
int Response::method_Post(){
    
    if (_request.get_isFile() == true)    //it is a file
    {
            log.print(INFO,"IS a file ...." + this->_request.get_pathInfo() + "/" + this->_request.get_fileName(),RED,true);
            if (isCGI_extension(this->_request.get_fileName(), _request._config._cgi) && this->_request.get_pathInfo() == "cgi-bin")
            {
                log.print(INFO,"IS a CGI file",GREEN,true);
                _is_a_cgi = true;
                //extension file cgi
                CGI cgi(this->_request);
                cgi.init(1);
                int ret = cgi.execute();
                _body = cgi.getBody();      // Put result in the Body 
                log.print(INFO,"server : >>  result cgi execvu: \n" + _body + std::to_string(ret),YELLOW,true);
                _response_headers["Content-Type"] = "CGI/MINE";
                return (_stat = ret);
                //return 0;
                //DO the MAGIC HERE
                //Define class
                /*
                CGI cgi(file_, config_, config_.getHeaders(), config_.getBody());
                cgi.init(worker_id_);
                if ((status_code_ = cgi.execute()) > 400)
                    return status_code_;
                cgi.parseHeaders(headers_); // Put some values in the headers
                body_ = cgi.getBody();      // Put result in the Body 
                headers_["Content-Length"] = ft::to_string(body_.length());
                return status_code_;
                */
            } else {
                log.print(INFO,"IS NOT CGI file",RED,true);
                // std::string new_path = _request._config.get_root() + "/" + _request.get_pathInfo() + _request.get_fileName();
                // _file.set_path(new_path);
                // log.print(INFO,"New path of file",RED,true);
                File tmp_file;
                std::string ext_file, loc_file;
                ext_file = _request._config.get_root() + _request._config.get_upload()+"/"+_request.get_fileName();
                loc_file = _request._config.get_upload()+"/"+_request.get_fileName();
                log.print(INFO,"path file to upload: " + ext_file, YELLOW,true);
                tmp_file.set_path(ext_file);
                std::string str = _request.get_body();
                _body = str;
                pthread_mutex_lock(&g_write);
                if(!tmp_file.exists()){     // no -> file create (body) --> code 201
                    log.print(INFO,"Creating file : " + ext_file, YELLOW,true);
                    tmp_file.create(str);
                    _response_headers["Location"] = loc_file ;
                    if (_request.get_isChunked() == 1)
                        _stat = 226;
                    else
                        _stat = 201;
                }
                else {                      // si -> file append (body) --> code 200
                    log.print(INFO,"Append to file : " + ext_file, YELLOW,true);
                    tmp_file.append(str);
                    _response_headers["Location"] = loc_file ;
                    _stat = 200;
                }
                pthread_mutex_unlock(&g_write);
                tmp_file.close();
                tmp_file.unlink();
            }
            //_file.set_path(_request._config.get_root() + _request.get_pathCGI() + "/" + _request.get_fileCGI());
    }
 
    
    //file exits? 
    // File tmp_file;
    // std::string ext_file, loc_file;
    // ext_file = _request._config.get_root() + _request._config.get_upload()+"/"+_request.get_fileName();
    // loc_file = _request._config.get_upload()+"/"+_request.get_fileName();
    // log.print(INFO,"path file to upload: " + ext_file, YELLOW,true);
    // tmp_file.set_path(ext_file);
    // std::string str = _request.get_body();
    // _body = str;
    // pthread_mutex_lock(&g_write);
    // if(!tmp_file.exists()){     // no -> file create (body) --> code 201
    //     log.print(INFO,"Creating file : " + ext_file, YELLOW,true);
    //     tmp_file.create(str);
    //     _response_headers["Location"] = loc_file ;
    //     if (_request.get_isChunked() == 1)
    //         _stat = 226;
    //     else
    //         _stat = 201;
    // }
    // else {                      // si -> file append (body) --> code 200
    //      log.print(INFO,"Append to file : " + ext_file, YELLOW,true);
    //     tmp_file.append(str);
    //     _response_headers["Location"] = loc_file ;
    //     _stat = 200;
    // }
    // pthread_mutex_unlock(&g_write);
    // tmp_file.close();
    // tmp_file.unlink();
    return 0;
}

int Response::method_Get(){
    //if directory and autoindex --> look for autoindex, change header Content_Type = html
    if (_request._config.get_autoindex()=="on" && _request.get_isFile() == false) // &&is directory
    {
        log.print(INFO,"IS a Autoindex:" +  _request._config.get_root() + " " + _request.get_pathInfo(),GREEN,true);
        std::string root = _request._config.get_root();
        std::string path = _request.get_pathInfo();
        if (path == "/")
            path = "";
        _body = autoindex(root, path);
        std::cout << _body << std::endl;
        return 0;
    }
    else   //is a file or a path?
    {    
        if (_request.get_isFile() == true)    //it is a file
        {
            log.print(INFO,"IS a file ...." + this->_request.get_fileName() +".."+ this->_request.get_pathInfo(),RED,true);
            if (isCGI_extension(this->_request.get_fileName(), _request._config._cgi) && this->_request.get_pathInfo() == "cgi-bin")
            {
                log.print(INFO,"IS a CGI file",GREEN,true);
                _is_a_cgi = true;
                //extension file cgi
                CGI cgi(this->_request);
                cgi.init(1);
                int ret = cgi.execute();
                _body = cgi.getBody();      // Put result in the Body 
                log.print(INFO,"server : >>  result cgi execvu: " + _body + std::to_string(ret),YELLOW,true);
                _stat = ret;
                return 0;
                //DO the MAGIC HERE
                //Define class
                /*
                CGI cgi(file_, config_, config_.getHeaders(), config_.getBody());
                cgi.init(worker_id_);
                if ((status_code_ = cgi.execute()) > 400)
                    return status_code_;
                cgi.parseHeaders(headers_); // Put some values in the headers
                body_ = cgi.getBody();      // Put result in the Body 
                headers_["Content-Length"] = ft::to_string(body_.length());
                return status_code_;
                */
            } else {
                log.print(INFO,"IS NOT CGI file",RED,true);
                std::string new_path = _request._config.get_root() + "/" + _request.get_pathInfo() + _request.get_fileName();
                _file.set_path(new_path);
                log.print(INFO,"New path of file",RED,true);
            }
            //_file.set_path(_request._config.get_root() + _request.get_pathCGI() + "/" + _request.get_fileCGI());
        }
        else //is a path must join default index.html
            _file.set_path(_request._config.get_root() + _request.get_pathInfo() + "/" + _request._config.get_index());
        
        log.print(INFO,"path to file: "+ _file.getPath() ,GREEN,true); 
        
        // if the file not exit error 404
        if (!_file.exists()){
            _stat = 404;
            return 0;
        }
        // if the file not open exit error 403 (permission denied)
        if (!_file.open()){
            _stat = 403;
            return 0;
        }
        _body = _file.getContent(); 
        _response_headers["Last-Modified"] = _file.last_modified();
    }
    _stat = 200;
    return 0;
    //_contentLength = _body.length();
}

std::string Response::autoindex(std::string &root, std::string &path){
    std::string body;
    struct tm *tm;
    char buf[32];
    DIR *dir;
    struct stat statbuf;
    struct dirent *ent;

    std::string target;
    target = root +_request.get_pathInfo();
    dir = opendir(target.c_str());

    //title
    body += "<html>\r\n";
    body += "<head><title>Index of " + target + "</title></head>\r\n";
    body += "<body>\r\n";
    body += "<h1>Index of " + target + "</h1><hr><pre>";
    
    std::vector<auto_listing> listing;
     
    if (!dir){
        body += "DIRECTORIO NO EXISTE</pre><hr></body>\r\n";
        body += "</html>\r\n";
        return body;
    }

    while ((ent = readdir(dir))) {
        auto_listing li;
        li.name = ent->d_name;
        if (li.name.length() > 50) {
            li.name.erase(47);
            li.name += "..>";
        }
        std::string path_tmp(target + "/" + ent->d_name);
        stat(path_tmp.c_str(), &statbuf);
        // struct stat {
        //        dev_t     st_dev;     /* ID of device containing file */
        //        ino_t     st_ino;     /* inode number */
        //        mode_t    st_mode;    /* protection */
        //        nlink_t   st_nlink;   /* number of hard links */
        //        uid_t     st_uid;     /* user ID of owner */
        //        gid_t     st_gid;     /* group ID of owner */
        //        dev_t     st_rdev;    /* device ID (if special file) */
        //        off_t     st_size;    /* total size, in bytes */
        //        blksize_t st_blksize; /* blocksize for file system I/O */
        //        blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
        //        time_t    st_atime;   /* time of last access */
        //        time_t    st_mtime;   /* time of last modification */
        //        time_t    st_ctime;   /* time of last status change */
        //    };
        li.is_dir = false;
        if (S_ISDIR(statbuf.st_mode)) {
            li.is_dir = true;
            li.name += "/";
        }
        tm = gmtime(&statbuf.st_mtime);
        int ret = strftime(buf, 32, "%d-%b-%Y %H:%M", tm);
        li.date = std::string(buf, ret);
        li.size = statbuf.st_size;
        //std::cout << "kk" << li.size << std::endl;
        listing.push_back(li);
    }
    ///std::sort(listing.begin(), listing.end(), sort_auto_listing);
    //printing list in body
    for (std::vector<auto_listing>::iterator it = listing.begin(); it != listing.end(); it++) {
        //body = body + "<a href=\"" + ft::unique_char(target +  "/" + it->name) + "\">" + it->name + "</a>";
        std::cout << "AUTOINDEX\n";
        std::cout << root << std::endl;
        body = body + "<a href=\"" + _request.get_actualLocation() + "/" + it->name + "\">" + it->name + "</a>";
        body += " ";
        body += it->date;  //date of file
        body += " ";
        if (it->is_dir)
            body += "-";
                //body += set_width(20, "-");
        else
            body += std::to_string(it->size);
        body += "\r\n";
    }

    body += "</pre><hr></body>\r\n";
    body += "</html>\r\n";
    closedir(dir);
    _stat = 200;
    return body;
}

void Response::setResponse()
{

    this->_response = this->_header;
    this->_response += "\n\n";
    //if (m_type != REDIRECT)
        this->_response += this->_body;
}

// Send response
int Response::sendResponse(int socket)
{
    // creating a body
    build_body();
    char *hello = "Testing Hello World\r\n";//IMPORTANT! WE WILL GET TO IT
    
    std::string m_body;  
    //m_body = hello;  
    m_body = _body; 
    this->_contentLength = m_body.length();  //Update body length
    setHeader();                             //Update header
    this->_response = this->_header;
    this->_response += m_body;
    //std::cout << _response << std::endl;
    return (write(socket, this->_response.c_str(), this->_response.length()));
    //close(client_socket);
}

// Check if method is allow 0 false 1 true
bool Response::is_allow_method(std::string met){
    int cont = 0;
    //std::cout <<_config._limits_except.size() << std::endl;
    while (cont < _request._config._limits_except.size())
    {
        //std::cout << _request._config._limits_except.at(cont) << std::endl;
        if (met == _request._config._limits_except.at(cont))
            return 1;
        cont++;
    }
    return 0;
}

//Check if is CGI 
bool Response::isCGI(std::string extension) {
  /*
  std::map<std::string, std::string> &cgi = config_.getCGI();

  for (std::map<std::string, std::string>::iterator it = cgi.begin(); it != cgi.end(); it++) {
    if (it->first == extension)
      return true;
  }
  */
  return false;
}