// sudo clang++ test.cpp SimpleSocket.cpp SimpleServer.cpp Request.cpp

#include "get_next_line.hpp"
#include "parse_config_server.hpp"
#include "parse_options.hpp"
#include "logger.hpp"
#include "page_errors.hpp"
#include "utils.hpp"
#include "SimpleSocket.hpp"
#include "SimpleServer.hpp"
#include "Request.hpp"
//#include "Cgi.hpp"
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
    pthread_t   _thr;
};

std::vector<Server> g_servers;

void *start_server(void *arg)
{
    SimpleServer myserver(AF_INET, SOCK_STREAM, IPPROTO_TCP,  ((Server*)arg)->_conf, INADDR_ANY);
    //myserver.launch();
    return NULL;
}

int main(int argc, char **argv)
{
    // std::string tmp("       ddfjhj  HHHjjj  ");
    // std::string tmp1;
    
    // tmp1 = ft_clean_token(tmp);
    // std::cout << tmp1 << std::endl;
//     std::cout << tmp1 << std::endl;

    //return 0;
    Parse_options myoptions(argc, argv);

    if (myoptions.parse())
    {
        std::cout << "FAIL:" << std::endl;
        std::cout << myoptions.helpText() << std::endl;
        return(0);
    }
    if (myoptions.help())
    {
        std::cout << myoptions.helpText() << std::endl;
        return (0);
    }

    log.print(INFO,"------------------------------ PARSE OPTIONS DONE ---------------------------------- ",YELLOW,true);
    
    // Verify correct config file
    Parse_config my_cluster(myoptions.get_path());
    if (!my_cluster.parse())
    {
        std::cout << "\nError en config:" << std::endl;
        return(0);
    }

    log.print(INFO,"------------------------------ "+ std::to_string(my_cluster.get_nb_servers())  + ": SERVERS DETECTED ---------------------------------- ",YELLOW,true);
    std::cout << my_cluster.get_nb_servers() << " servers detected." << std::endl;

    // show each config raw data
    int c = 0;
    while (c < my_cluster.get_nb_servers())
    {   
        log.print(INFO,"Servidor:" + std::to_string(c),GREEN,true);
        my_cluster.show_config_one(c);
        c++;
    }

    log.print(INFO,"------------------------------ PARSE RAW CONFIG FILE LOAD FROM :" + myoptions.get_path() + "------------------------",YELLOW,true);
   
    //main loop
    /*
        while (1)
        {
            read_fds = master; // copy it
            if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
            {
                        perror("select");
                        exit(4);
            }
            // run through the existing connections looking for data to read
            for(i = 0; i <= fdmax; i++) 
            {
                //check incoming data
                if (FD_ISSET(i, &read_fds)) // we got one!! FD_ISSET == TRUE
                { 
                    if (i == fd_listen) 
                    {
                        // handle new connections
                        addrlen = sizeof remoteaddr;
                        newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
                        if (newfd == -1) 
                        {
                            perror("accept");
                        } 
                        else 
                        {
                            FD_SET(newfd, &master);         // add to master set
                            if (newfd > fdmax)              // keep track of the max
                            {    
                                fdmax = newfd;
                            }
                            printf("selectserver: new connection from %s on socket %d\n", inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN), newfd);
                        }
                    } 
                    else 
                    {   // handle data from a client
                        if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                            // got error or connection closed by client
                            if (nbytes == 0) {
                                // connection closed
                                printf("selectserver: socket %d hung up\n", i);
                            } else {
                                perror("recv");
                            }
                            close(i);           // close connection
                            FD_CLR(i, &master); // remove from master set
                        } 
                        else 
                        {
                            // we got some data from a client
                            // do something....
                            //for example send to everyone
                            for(j = 0; j <= fdmax; j++) {
                                // send to everyone!
                                if (FD_ISSET(j, &master)) {
                                    // except the listener and ourselves
                                    if (j != listener && j != i) {
                                        if (send(j, buf, nbytes, 0) == -1) {
                                            perror("send");
                                        }
                                    }
                                }
                            }
                        }
                    } // END handle data from client   
                } // END got new incoming connection
            } //END looping through file descriptors
        } // END infinte loop it would never end!

*/    
    
    
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