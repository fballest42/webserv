// Parsing directives config of a server 
//  -listen address[:port]; ó port;   //bing the given addres to the port. 
//        if no addreess is given binds 0.0.0.0. 
//        if no port, binds to 80.
//  -server_name name ...; //sets names of a virtual server
//  -root path;  //sets the directory for requests.
//  -auth login:password; //restricts a route for a user. [NOT IMPLEMENTED]
//  -autoindex on|off; (default off) //enables or disables the directory listing out`put
//  -index file ....;    //defines file that will be used as an index
//  -Error_page code ... uri; //defines the URI that will be shown for the specified errors.
//  -limit_except GET ...; //defines method accepted
//  -upload
//  -cgi extension cgi_path;     //defines files that will be executed for given extension
//  -cgi_bin folder_path; (default ./cgi-bin)    //define a folder where to search CGI binaries
//  -client_max_body_size ....; //define max body size
//  -location

// ERRORS:
// root -> only one value if multiple definition take the last one
// upload -> only one value if multiple definition take the last one
// client_max_body_size one value if multiple definition take the last one
// autoindex only one value: on/off.
// Check is integer in port error_page client_max_body

#include "config.hpp"

extern Logger log;

Config::Config(std::vector<std::string> vector_cfg, int nb):  _vector_cfg(vector_cfg), _nb(nb)
{
    _autoindex = "off";
}

Config::Config()  
{
    _autoindex = "off";
}
void Config::set_Config(std::vector<std::string> vector_cfg, int nb)
{
    _vector_cfg = vector_cfg;
    _nb = nb;
}

//getters
std::string Config::get_root(void)
{ return (_root); }

std::string Config::get_index(void)
{ return (_index.at(0)); }

bool Config::check_location(std::map<std::string, std::vector<std::string> >  locations)
{
    for (std::map<std::string, std::vector<std::string> >::iterator it=locations.begin(); it!=locations.end(); ++it)
    {
        int c = 0;
        bool is_valid;
        std::string token;
        std::string location;
        std::vector<std::string> location_vector;
        int n_tokens;
        int n_cgi_bin = 0;
        bool size  = false;
        bool root_exits = false;
        std::vector<std::string> vector_cfg = it->second;
        while(c < vector_cfg.size())
        {
            token = get_token(vector_cfg.at(c), ' ', 0); 
            n_tokens = nb_tokens(vector_cfg.at(c), ' ');
            is_valid = false;
            if (token == "cgi" && n_tokens == 3)   //defines files that will be executed for given extension "cgi .sh sh;
               is_valid = true; 
            if (token == "cgi-bin" && n_tokens == 2)   //(default ./cgi-bin)    //define a folder where to search CGI binaries "cgi_bin folder_path;
            {   
                n_cgi_bin ++;
                if (n_cgi_bin > 1) 
                    is_valid = false;
                else {
                    is_valid = true;
                }
            }
            if (token == "error_page" && n_tokens == 3)   //check number of tokens pendiente
                {
                    if (!is_number(get_token(_vector_cfg.at(c), ' ', 1)))
                    {
                        log.print(INFO," [ERROR: Config file: error code page is not a number]",RED,true);
                        return(false);
                    } 
                    is_valid = true;
                }   
            if (token == "root") //&& n_tokens == 2)   //check number of tokens pendiente
                {
                    if   (n_tokens > 2)        
                    {
                        log.print(INFO," [ERROR: Config file: double value in 'root' directive]",RED,true);
                        return(false);
                    }
                    root_exits = true;
                    is_valid = true;
                }
            if (token == "index" && n_tokens == 2)   //check number of tokens pendiente
                {   
                    int cont = 1;
                    while (cont < nb_tokens(vector_cfg.at(c),' '))
                        cont++;
                    is_valid = true;
                }
            if (token == "autoindex" && n_tokens == 2)   //check number of tokens pendiente
                {            
                    if   (n_tokens > 2)        
                    {
                        log.print(INFO," [ERROR: Config file: double value in 'autoindex' directive]",RED,true);
                        return(false);
                    }
                    if (get_token(vector_cfg.at(c), ' ', 1) != "on" && get_token(vector_cfg.at(c), ' ', 1) != "off")
                    {
                        log.print(INFO," [ERROR: Config file: unknown value in 'autoindex' directive]",RED,true);
                        return(false);
                    }
                    is_valid = true;
                }
            if (token == "upload") // && n_tokens == 2)   //only one;
                {
                    if   (n_tokens > 2)      
                    {
                        log.print(INFO," [ERROR: Config file: double value in 'upload' directive]",RED,true);
                        return(false);
                    }
                    is_valid = true;
                }
            if (token == "client_max_body_size" && n_tokens == 2)   //check number of tokens pendiente
                { 
                    if   (n_tokens > 2) 
                    {
                        log.print(INFO," [ERROR: Config file: double value in 'client_max_body_size' directive]",RED,true);
                        return(false);
                    }
                    if (!is_number(get_token(vector_cfg.at(c), ' ', 1)))
                    {
                        log.print(INFO," [ERROR: Config file: client_max_body_size is not a number]",RED,true);
                        return(false);
                    } 
                    size = true;
                    is_valid = true;
                } 
            if (token == "limit_except")
                {
                    int cont = 1;
                    while (cont < nb_tokens(vector_cfg.at(c), ' '))
                        cont++;
                    is_valid = true;
                }
            if (is_valid == false)
            {
                log.print(INFO," [ERROR: Config file: directive not valid -> " + token + "]",RED,true);
                return(false);
            }
            c++;
        }
    }
    
    return true;
}

// si correcto true else false
int Config::parse(void)
{
    int c = 0;
    bool is_valid;
    std::string token;
    std::string token2;
    std::string location;
    std::vector<std::string> location_vector;
    int n_tokens;
    int n_cgi_bin = 0;
    bool size  = false;
    bool root_exits = false;

    while(c < _vector_cfg.size())
    {
        token = get_token(_vector_cfg.at(c), ' ', 0); 
        n_tokens = nb_tokens(_vector_cfg.at(c), ' ');
        is_valid = false;
        if (token == "listen" && n_tokens == 2)  
            {
                std::string new_token = get_token(_vector_cfg.at(c), ' ', 2);
                if ((nb_tokens(new_token, ':') == 2))
                {
                    std::string new_port = get_token(new_token, ':', 2);
                    if (!is_number(new_port))
                    {
                        log.print(INFO," [ERROR: Config file: port is not a number]",RED,true);
                        return(false);
                    }    
                    _ports.push_back(stoi(new_port));
                    is_valid = true;
                }
                else {
                    if (!is_number(new_token))
                    {
                        log.print(INFO," [ERROR: Config file: port is not a number]",RED,true);
                        return(false);
                    }    
                    _ports.push_back(stoi(new_token));
                    is_valid = true;
                }
            }
        if (token == "cgi" && n_tokens == 3)   //defines files that will be executed for given extension "cgi .sh sh;
        {
            _cgi[get_token(_vector_cfg.at(c), ' ', 1)] = get_token(_vector_cfg.at(c), ' ', 2);
            is_valid = true;
        }  
        if (token == "cgi-bin" && n_tokens == 2)  //define a folder where to search CGI binaries "cgi_bin folder_path;
        {   
            _cgi_bin = get_token(_vector_cfg.at(c), ' ', 2);
            n_cgi_bin ++;
            if (n_cgi_bin > 1) 
                is_valid = false;
            else {
                if (_cgi_bin[0]=='/')
                    _cgi_bin = _cgi_bin.substr(1,_cgi_bin.size());
                if (_cgi_bin[_cgi_bin.size()-1]=='/')
                    _cgi_bin= _cgi_bin.substr(0,_cgi_bin.size()-1);
                is_valid = true;
            }
        }
        if (token == "error_page" && n_tokens == 3)   
            {
                if (!is_number(get_token(_vector_cfg.at(c), ' ', 1)))
                {
                    log.print(INFO," [ERROR: Config file: error code page is not a number]",RED,true);
                    return(false);
                } 
                _error_pages[stoi(get_token(_vector_cfg.at(c), ' ', 1))] = get_token(_vector_cfg.at(c), ' ', 2);
                is_valid = true;
            }   
        if (token == "root") //&& n_tokens == 2)  
            {
                if   (n_tokens > 2)        
                {
                    log.print(INFO," [ERROR: Config file: double value in 'root' directive]",RED,true);
                    return(false);
                }
                _root = (get_token(_vector_cfg.at(c), ' ', 1));
                root_exits = true;
                is_valid = true;
            }
        if (token == "server_name" )  
            {
                int cont = 1;
                while (cont < nb_tokens(_vector_cfg.at(c), ' '))
                {   _server_names.push_back(get_token(_vector_cfg.at(c), ' ', cont));
                    cont++;
                }
                is_valid = true;
            } 
        if (token == "index" && n_tokens == 2)   
            {   
                int cont = 1;
                while (cont < nb_tokens(_vector_cfg.at(c),' '))
                {   
                    _index.push_back(get_token(_vector_cfg.at(c), ' ', cont));
                    cont++;
                }
                is_valid = true;
            }
        if (token == "autoindex" && n_tokens == 2)  
            {            
                if   (n_tokens > 2)        
                {
                    log.print(INFO," [ERROR: Config file: double value in 'autoindex' directive]",RED,true);
                    return(false);
                }
                if (get_token(_vector_cfg.at(c), ' ', 1) != "on" && get_token(_vector_cfg.at(c), ' ', 1) != "off")
                {
                    log.print(INFO," [ERROR: Config file: unknown value in 'autoindex' directive]",RED,true);
                    return(false);
                }
                _autoindex = get_token(_vector_cfg.at(c), ' ', 1);
                is_valid = true;
            }
        if (token == "upload") // && n_tokens == 2)   //only one;
            {
                if   (n_tokens > 2)      
                {
                    log.print(INFO," [ERROR: Config file: double value in 'upload' directive]",RED,true);
                    return(false);
                }
                _upload  = get_token(_vector_cfg.at(c), ' ', 1);
                if (_upload[_upload.length()-1]=='/')
                    _upload = _upload.substr(0,_upload.length()-1);
                if (_upload[0]=='/')
                    _upload = _upload.substr(1,_upload.length());
                is_valid = true;
            }
        if (token == "client_max_body_size" && n_tokens == 2)   //check number of tokens pendiente
            { 
                if   (n_tokens > 2) 
                {
                    log.print(INFO," [ERROR: Config file: double value in 'client_max_body_size' directive]",RED,true);
                    return(false);
                }
                if (!is_number(get_token(_vector_cfg.at(c), ' ', 1)))
                {
                    log.print(INFO," [ERROR: Config file: client_max_body_size is not a number]",RED,true);
                    return(false);
                } 
                _client_max_body_size = (stol(get_token(_vector_cfg.at(c), ' ', 1)));
                size = true;
                is_valid = true;
            } 
        if (token == "limit_except")
            {
                int cont = 1;
                while (cont < nb_tokens(_vector_cfg.at(c), ' '))
                {   _limits_except.push_back(get_token(_vector_cfg.at(c), ' ', cont));
                    cont++;
                }
                is_valid = true;
            }
        if ((token == "location" && n_tokens == 3))   
        {
           if (get_token(_vector_cfg.at(c), ' ', 1) == "/")
                location = "/";
            else
            {
                std::string loc = get_token(_vector_cfg.at(c), ' ', 1);
                if (loc[loc.size()-1]=='/')
                    loc.pop_back();
                location = loc;
            }
           c++;
           //carga data de location
           while (1)
           {
                location_vector.push_back(_vector_cfg.at(c));
                c++;
                if (get_token(_vector_cfg.at(c), ' ', 1) == "}")
                    break;
            } 
           _locations[location]=location_vector;
           location_vector.clear();
           is_valid = true;
        }  
        
        if (is_valid == false)
        {
            log.print(INFO," [ERROR: Config file: directive not valid -> " + token + "]",RED,true);
            return(false);
        }
        c++;
    }

    //CHECK LOCATIONS
    if (!check_location(_locations))
        return false;

    //// SETTING DEFAULT VALUES 
    if (_ports.empty())
        _ports.push_back(80);   // puerto 80 default
    if (_limits_except.empty()){
        _limits_except.push_back("POST");
        _limits_except.push_back("DELETE");
        _limits_except.push_back("GET");
    }
    if (_index.empty())
        _index.push_back("index.html");
    if (_server_names.empty())
        _server_names.push_back("localhost");    
    if (!size)
        _client_max_body_size=30000000000; //NGINX dfault size?
    return(true);
}

void Config::show_config(void)
{
    int c = 0;
    while(c<_vector_cfg.size())
    {
        std::cout << _vector_cfg.at(c) << std::endl; 
        c++;
    }
    return;
}

void Config::show_ports(void)
{
     std::cout << "--- PORTS ---" << std::endl;
    int c = 0;
    while(c < _ports.size())
        std::cout << "ports:" << _ports.at(c++)  << std::endl;
    return;
}

int Config::port(int p)
{
    return _ports.at(p);
}

void Config::show_cgi(void)
{
    std::cout << "--- CGI ---" << std::endl;
    for (std::map<std::string, std::string>::iterator it=_cgi.begin(); it!=_cgi.end(); ++it)
        std::cout << it->first << " => " << it->second << '\n';
    return;
}

void Config::show_cgi_bin(void)
{
    std::cout << "--- CGI_BIN ---" << std::endl;
    std::cout << _cgi_bin << std::endl;
    return;
}

void Config::show_error_pages(void)
{
     std::cout << "--- ERROR PAGES ---" << std::endl;
    for (std::map<int, std::string>::iterator it=_error_pages.begin(); it!=_error_pages.end(); ++it)
        std::cout << it->first << " => " << it->second << '\n';
    return;
}

void Config::show_roots(void)
{
     std::cout << "--- ROOT ---" << std::endl;

        std::cout << _root << '\n';
 
}

void Config::show_index(void)
{
     std::cout << "--- INDEX ---" << std::endl;
    int c = 0;
    while (c < _index.size())
    {
        std::cout << _index.at(c) << '\n';
        c++;
    }
    return;
}

void Config::show_autoindex(void)
{
     std::cout << "--- AUTOINDEX ---" << std::endl;
     std::cout << _autoindex << '\n';
    return;
}

void Config::show_server_names(void)
{
     std::cout << "--- SERVER_NAMES ---" << std::endl;
    int c = 0;
    while (c < _server_names.size())
    {
        std::cout << _server_names.at(c) << '\n';
        c++;
    }
    return;
}

void Config::show_limit_except(void)
{
     std::cout << "--- LIMIT_EXCEPT ---" << std::endl;
    int c = 0;
    
    while (c < _limits_except.size())
    {
        std::cout << _limits_except.at(c) << '\n';
        c++;
    }
    
    return;
}

void Config::show_locations(void)
{
     std::cout << "--- LOCATIONS ---" << std::endl;
    std::cout <<  " number => " << _locations.size() << '\n';
    if (_locations.size() > 0){
        for (std::map<std::string, std::vector<std::string> >::iterator it = _locations.begin(); it!=_locations.end();++it)
        {
            std::cout << it->first << " = > " << std::endl;
            int c = 0;
            while (c < it->second.size())
            {
                std::cout << it->second.at(c) << '\n';
                c++;
            }
        }
    }    
    return;
}

void Config::show_client_max_body(void)
{
    std::cout << "--- CLIENT MAX BODY ---" << std::endl;
    std::cout <<  std::to_string(_client_max_body_size) << '\n';
}

void Config::show_upload(void)
{
    std::cout << "--- UPLOAD ---" << std::endl;
    std::cout <<  _upload << '\n';
    return;
}

void Config::show_all(void){
    show_ports();
    show_server_names();
    show_roots();
    show_index();
    show_autoindex();
    show_client_max_body();
    show_error_pages();
    show_limit_except();
    show_cgi();
    show_cgi_bin();
    show_upload();
    show_locations();
}
// Return true if exits false if not
bool Config::exist_error_page(int code)
{
    std::map<int ,std::string>::iterator it; 
    it = _error_pages.find(code);
    if (it != _error_pages.end())
        return (true);
    return (false);
}
// Return page path for code error
std::string Config::get_path_error_page(int code)
{
    return (_error_pages.at(code));
}
