#include "parse_config_server.hpp" 

//construtor
Parse_config::Parse_config(std::string  path) : _path(path), _nb_servers(0){}

//Divide las configuraciones de varios servidores store in a vector of config and parse it
bool Parse_config::parse(void)
{
    int _fd;
    _fd = open(_path.c_str(), O_RDONLY);
    if (_fd < 0){
        throw WebServer_Exception("config file not found");
        return(false);
    }
    
    bool is_server = false;
    bool is_valid;
    bool is_finish = false;
    char *line;
    std::string s_out ="";
    int open_brakers = 0;
    int nb_w = 0;
    std::vector<std::string> tmp;

    while(get_next_line(_fd, &line))
    {
      
      std::string s(line);
      strip(s, s_out);                              //delete spaces not necesary
      if (!s_out.empty() && s_out.at(0) != '#')     //not empty lines and comment
      {              
            is_valid = false;
            if (get_token(s_out," ",0)=="workers")
            {
                _workers = stoi(get_token(s_out, " ", 1));
                is_valid = true;
                nb_w++;
            }

            if (get_token(s_out," ",0)=="server")
            {
                _nb_servers = _nb_servers + 1;
                is_server = true;
                is_valid = true;
            }
            
            if (is_server)
            {
                if (get_token(s_out," ", nb_tokens(s_out, " "))=="{")
                    open_brakers = open_brakers + 1;
                if (get_token(s_out," ", nb_tokens(s_out, " "))=="}")
                    open_brakers = open_brakers - 1;
                if (open_brakers == 0)
                {
                    is_finish = true;
                    is_server = false;
                }
                if (!s_out.empty() && s_out.at(0) != '#')
                {
                    if (s_out.at(s_out.size()-1) == ';')   //elimino el ; no lo necesito
                        s_out.pop_back();
                    tmp.push_back(s_out);
                }
                is_valid = true;
            }
            
            if (is_finish ==  true)
            {
                tmp.erase(tmp.begin()); //quito el primer {
                tmp.pop_back();         //quito el último }
                _server.push_back(tmp);
                tmp.clear();
                is_finish = false;
                is_valid = true;
            }
            
            if (is_valid == false) // && is_server == false) 
            {
                throw WebServer_Exception("directive not valid");
                return(false);
            }
            if (nb_w > 1) // too many workers definition
            {
                throw WebServer_Exception("too many workers definition");
                return(false);
            }
      }  
    }

    // Hay al menos un servidor definido?
    if (_nb_servers == 0)
        throw WebServer_Exception("missing server block");
    
    //carga each configuration server 
    int e = 0;
    while ( e < _nb_servers )
    {
        std::vector<std::string> tmp(_server.at(e));
        {
            Config cfg1(_server.at(e)); //yyyyborrar
            _configuration.push_back(cfg1);
        }
        e++;
    }

    e = 0;
    while ( e < _nb_servers )
    {
        if(!_configuration.at(e).parse())
            return(false);
        e++;
    }
    return true;
}

void Parse_config::show_config(void)
{
    int c = 0;
    int d;
    std::cout << "Servidores detectados:  " << _server.size() << std::endl;
    while(c<_server.size())
    {
        d = 0;
        
        std::vector<std::string> tmp(_server.at(c));
        std::cout << "Servidor Nº: " << c + 1 << std::endl<< std::endl;
        std::cout << "numero de registros:"<< tmp.size() << std::endl;
        while(d<_server.at(c).size())
        {
            std::cout << tmp.at(d) << std::endl; //<< " numbr of tokens: " << nb_tokens(_vector_cfg.at(c)," ") << std::endl;
            d++;
        }
        std::cout << std::endl;
        //std::cout << "servidor ;" << c << std::endl; //<< " numbr of tokens: " << nb_tokens(_vector_cfg.at(c)," ") << std::endl;
        c++;
    }
    return;
}

void Parse_config::show_config_one(int n)
{
    _configuration.at(n).show_ports();
    _configuration.at(n).show_roots();
    _configuration.at(n).show_index();
    _configuration.at(n).show_autoindex();
    _configuration.at(n).show_cgi();
    _configuration.at(n).show_cgi_bin();
    _configuration.at(n).show_error_pages();
    return;
}