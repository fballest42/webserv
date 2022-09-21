#include "get_next_line.hpp"
#include "parse_config_server.hpp"
#include "parse_options.hpp"
#include "logger.hpp"
#include "page_errors.hpp"
#include "utils.hpp"
#include "SimpleSocket.hpp"
#include "SimpleServer.hpp"
#include "Request.hpp"
#include "file.hpp"
#include <thread>
#include <string>

Logger log;
Error_page err_page;

pthread_mutex_t g_write;  

//Structure for passing arguments to threads
struct Server {
    int         _id;
    Config      _conf;
    std::map< int, std::map < std::string, Config > > _mycluster;
    pthread_t   _thr;
};

//Structure for passing arguments to threads
struct S_Ports {
    int         _port;
    std::map < std::string, Config > _myconfigs;
    pthread_t   _thr;
};

std::vector<Server> g_servers;
std::vector<S_Ports> g_ports;

void *start_ports(void *arg)
{
    SimpleServer myserver(AF_INET, SOCK_STREAM, IPPROTO_TCP, ((S_Ports*)arg)->_port, ((S_Ports*)arg)->_myconfigs, INADDR_ANY);
    return NULL;
}

int main(int argc, char **argv)
{
    Parse_options myoptions(argc, argv);

    if (myoptions.parse())
    {
        log.print(INFO," [HELP] ",GREEN,true);
        std::cout << myoptions.helpText() << std::endl;
        return(0);
    }
    if (myoptions.help())
    {
        std::cout << myoptions.helpText() << std::endl;
        return (0);
    }

    log.print(INFO," [PARSE OPTIONS DONE] ",GREEN,true);
    
    // Verify correct config file
    Parse_config my_cluster(myoptions.get_path());
    if (!my_cluster.parse())
    {
        log.print(INFO," [ERROR in config file: " + myoptions.get_path() + "]", RED,true);
        return(0);
    }

    log.print(INFO," ["+ std::to_string(my_cluster.get_nb_servers())  + ": servers detected ]",GREEN,true);
    // show each config raw data

    signal(SIGPIPE, SIG_IGN);

    int c = 0;
    while (c < my_cluster.get_nb_servers())
    {   
        log.print(INFO,"Configuracion del Servidor: " + std::to_string(c),GREEN,true);
        //my_cluster.show_config_one(c);
        c++;
    }
    log.print(INFO," [Config file load from: " + myoptions.get_path() + "]",GREEN,true);

    my_cluster.load_cluster();
   
    if (myoptions.get_test())
        exit(0);  

    log.print(INFO,"Starting Servers",GREEN,true);
    
    g_servers.resize(my_cluster.get_nb_servers());
    g_ports.resize(my_cluster._cluster.size());
    log.print(INFO,"Number of ports: " + std::to_string(my_cluster._cluster.size()),GREEN,true);
    int i = 0;
    for (std::map< int, std::map < std::string, Config > >::iterator it=my_cluster._cluster.begin(); it!=my_cluster._cluster.end(); ++it)
    {
        g_ports[i]._port = it->first;                                            
        g_ports[i]._myconfigs = it->second;                      
        pthread_create(&g_ports[i]._thr,NULL, start_ports ,&g_ports[i]);
        usleep(500);
        i++;
    }
    i = 0;
    for (std::map< int, std::map < std::string, Config > >::iterator it=my_cluster._cluster.begin(); it!=my_cluster._cluster.end(); ++it)
    {
        pthread_join(g_ports[i]._thr,NULL);
        i++;
    }
    pthread_mutex_destroy(&g_write);
    std::cout << "Stopping Servers" << std::endl;
    return (0);
}
