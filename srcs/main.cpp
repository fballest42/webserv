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

pthread_mutex_t g_write;   //mutex to write in screen

//Structure for passing arguments to threads
struct Server {
    int         _id;
    Config      _conf;
    pthread_t   _thr;
};

std::vector<Server> g_servers;

void *start_server(void *arg)
{
    SimpleServer myserver(AF_INET, SOCK_STREAM, IPPROTO_TCP,  ((Server*)arg)->_conf, INADDR_ANY);
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
        my_cluster.show_config_one(c);
        c++;
    }
    log.print(INFO," [Config file load from: " + myoptions.get_path() + "]",GREEN,true);
    if (myoptions.get_test())
        exit(0);  
    
    log.print(INFO,"Starting Servers",GREEN,true);
    g_servers.resize(my_cluster.get_nb_servers());                              // resize to the number of servers
    for (int i = 0; i < my_cluster.get_nb_servers(); i++)
    {
        g_servers[i]._id = i;                                                   // pass the number of the server
        g_servers[i]._conf = my_cluster.get_server(i);                          //
        pthread_create(&g_servers[i]._thr,NULL, start_server ,&g_servers[i]);
        usleep(500);
    }
    for (int i = 0; i < my_cluster.get_nb_servers(); i++)
    {
        pthread_join(g_servers[i]._thr,NULL);
    }
    pthread_mutex_destroy(&g_write);
    std::cout << "Stopping Servers" << std::endl;
    return (0);
}