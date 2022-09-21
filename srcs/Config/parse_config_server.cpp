// Check general configuration file 
//      workers 9;
//      server {
//          ...
//      }
//      server {
//          ...
//      }
// and load each server definition in a Config Class (vector<string>, nb)

#include "parse_config_server.hpp"
#include "utils.hpp" 

extern Logger log;

//constructor
Parse_config::Parse_config(std::string  path) : _path(path), _nb_servers(0){}

//Divide las configuraciones de varios servidores store in a vector of config and parse it
bool Parse_config::parse(void)
{
    int _fd;
    _fd = open(_path.c_str(), O_RDONLY);
    if (_fd < 0){
        log.print(INFO," [ERROR: Config file: " + _path + " not found]",RED,true);
        return(false);
    }
    
    bool is_server = false;
    bool is_valid;
    bool is_finish = false;
    char *line;
    std::string s_out ="";
    std::string s_outm ="";
    int open_brakers = 0;
    int nb_w = 0;
    std::vector<std::string> tmp;
    
    while(get_next_line(_fd, &line))
    { 
      std::string s(line);
      stripTab(s, s_outm);
      strip(s_outm, s_out);                              //delete spaces not necesary
      if (!s_out.empty() && s_out.at(0) != '#')         //not empty lines and comment
      {              
            is_valid = false;
            if (get_token(s_out,' ',0)=="workers")
            {
                _workers = stoi(get_token(s_out, ' ', 1));
                is_valid = true;
                nb_w++;
            }

            if (get_token(s_out, ' ',0)=="server")
            {
                _nb_servers = _nb_servers + 1;
                is_server = true;
                is_valid = true;
            }
            
            if (is_server)
            {
                if (get_token(s_out, ' ', nb_tokens(s_out, ' '))=="{")
                    open_brakers = open_brakers + 1;
                if (get_token(s_out, ' ', nb_tokens(s_out, ' '))=="}")
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
            
            if (is_valid == false)
            {
                log.print(INFO," [ERROR: Config file: directive not valid]",RED,true);
                return(false);
            }
            if (nb_w > 1) // too many workers definition
            {
                log.print(INFO," [ERROR: Config file: too many workers definition]",RED,true);
                return(false);
            }
      }  
    }

    if (open_brakers!=0)
    {
        log.print(INFO," [ERROR: Config file: not a close block]",RED,true);
        return(false);
    }

    // Hay al menos un servidor definido?
    if (_nb_servers == 0){
        log.print(INFO," [ERROR: Config file: missing server block]",RED,true);
        return(false);
    }
   
    //carga each configuration server  in a matriz of config 
    int e = 0;
    while ( e < _nb_servers )
    {
        Config cfg1(_server.at(e), e);
        _configuration.push_back(cfg1);
        e++;
    }
    //Check each config is ok
    e = 0;
    while ( e < _nb_servers )
    {
        if(!_configuration.at(e).parse())
            return(false);
        e++;
    }
    return true;
}

void Parse_config::load_cluster()
{
    std::map < std::string, Config > tmp_map;

    for (std::vector<Config>::iterator it_cluster = _configuration.begin() ; it_cluster != _configuration.end(); ++it_cluster)
    {
        tmp_map.clear();
        int c = 0;
        while(c < it_cluster->get_ports().size())
        {
            int d = 0;
            while(d < it_cluster->get_server_names().size())
            {
                tmp_map[it_cluster->get_server_names().at(d)] = *it_cluster; 
                //check if port exist
                if (_cluster.find(it_cluster->get_ports().at(c)) == _cluster.end()) // || _request_headers["host"].empty())
                    _cluster[it_cluster->get_ports().at(c)] = tmp_map;
                else 
                    _cluster[it_cluster->get_ports().at(c)].insert( std::pair<std::string, Config>(it_cluster->get_server_names().at(d), *it_cluster));
                d++;
            }
            c++;
        }
    }
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
        c++;
    }
    return;
}

void Parse_config::show_config_one(int n)
{
    _configuration.at(n).show_all();
    return;
}
