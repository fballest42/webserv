#include "config.hpp"

Config::Config(std::vector<std::string> vector_cfg):  _vector_cfg(vector_cfg)
{
    _ports.push_back(80);
    _autoindex.push_back("off");
    
}

// si correcto true else false
int Config::parse(void)
{
       // get tokens
    int c = 0;
    bool is_valid;
    std::string token;
    int n_tokens;
    while(c < _vector_cfg.size())
    {
        token = get_token(_vector_cfg.at(c), " ", 0);
        n_tokens = nb_tokens(_vector_cfg.at(c), " ");
        is_valid = false;
        if (token == "listen" && n_tokens == 2)   //check number of tokens pendiente
        {
            _ports.push_back(stoi(get_token(_vector_cfg.at(c), " ", 1)));
            is_valid = true;
        }
        if (token == "cgi" && n_tokens == 3)   //check number of tokens pendiente
        {
            _cgi[get_token(_vector_cfg.at(c), " ", 1)] = get_token(_vector_cfg.at(c), " ", 2);
            is_valid = true;
        }  
        if (token == "cgi_bin" && n_tokens == 3)   //check number of tokens pendiente
        {   
            _cgi_bin[get_token(_vector_cfg.at(c), " ", 1)] = get_token(_vector_cfg.at(c), " ", 2);
            std::cout << std::endl;
            is_valid = true;
        }
        if (token == "error_page" && n_tokens == 3)   //check number of tokens pendiente
        {
            _error_pages[stoi(get_token(_vector_cfg.at(c), " ", 1))] = get_token(_vector_cfg.at(c), " ", 2);
            is_valid = true;
        }   
        if (token == "root" && n_tokens == 2)   //check number of tokens pendiente
        {
            int cont = 1;
            //std::cout << " => " << nb_tokens(_vector_cfg.at(c)," ") << '\n';
 
            while (cont < nb_tokens(_vector_cfg.at(c)," "))
            {   _roots.push_back(get_token(_vector_cfg.at(c), " ", cont));
                cont++;
            }
            is_valid = true;
        }
        if (token == "server_name" )   //check number of tokens pendiente
        {
            int cont = 1;
            //std::cout << " => " << nb_tokens(_vector_cfg.at(c)," ") << '\n';
 
            while (cont < nb_tokens(_vector_cfg.at(c)," "))
            {   _server_names.push_back(get_token(_vector_cfg.at(c), " ", cont));
                cont++;
            }
            is_valid = true;
        } 
        if (token == "index")   //check number of tokens pendiente
        {   
            int cont = 1;
            //std::cout << " => " << nb_tokens(_vector_cfg.at(c)," ") << '\n';
 
            while (cont < nb_tokens(_vector_cfg.at(c)," "))
            {   _index.push_back(get_token(_vector_cfg.at(c), " ", cont));
                cont++;
            }
            is_valid = true;
        }
        if (token == "autoindex" && n_tokens == 2)   //check number of tokens pendiente
        {
            int cont = 1;
            //std::cout << " => " << nb_tokens(_vector_cfg.at(c)," ") << '\n';
            while (cont < nb_tokens(_vector_cfg.at(c)," "))
            {   _autoindex.push_back(get_token(_vector_cfg.at(c), " ", cont));
                cont++;
            }
            is_valid = true;
        }
        if (token == "upload" )   //check number of tokens pendiente
        {
            is_valid = true;
            std::cout << std::endl;
        }
        
        //if (token == "location" && n_tokens == 3)   //check number of tokens pendiente
        //{
        //    _locations[get_token(_vector_cfg.at(c), " ", 1)] = get_token(_vector_cfg.at(c), " ", 2);
        //    is_valid = true;
        //}  
        
        if (is_valid == false)
        {
            throw WebServer_Exception("directive not valid");
            return(false);
        }
        c++;
    }
    return(true);
}

void Config::show_config(void)
{
    int c = 0;
    while(c<_vector_cfg.size())
    {
        std::cout << _vector_cfg.at(c) << std::endl; //<< " numbr of tokens: " << nb_tokens(_vector_cfg.at(c)," ") << std::endl;
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
    for (std::map<std::string, std::string>::iterator it=_cgi_bin.begin(); it!=_cgi_bin.end(); ++it)
        std::cout << it->first << " => " << it->second << '\n';
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
    //std::cout <<  " => " << _roots.size() << '\n';
    //exit(1);
     std::cout << "--- ROOTS ---" << std::endl;
    int c = 0;
    while (c < _roots.size())
    {
        std::cout << _roots.at(c) << '\n';
        c++;
    }
    return;
}

void Config::show_index(void)
{
    //std::cout <<  " => " << _roots.size() << '\n';
    //exit(1);
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
    //std::cout <<  " => " << _roots.size() << '\n';
    //exit(1);
     std::cout << "--- AUTOINDEX ---" << std::endl;
    int c = 0;
    while (c < _autoindex.size())
    {
        std::cout << _autoindex.at(c) << '\n';
        c++;
    }
    return;
}

void Config::show_server_names(void)
{
     std::cout << "--- SERVER_NAMES ---" << std::endl;
    //std::cout <<  " => " << _roots.size() << '\n';
    //exit(1);
    int c = 0;
    while (c < _server_names.size())
    {
        std::cout << _server_names.at(c) << '\n';
        c++;
    }
    return;
}