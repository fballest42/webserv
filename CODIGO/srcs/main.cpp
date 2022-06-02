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

Logger log;
Error_page err_page;

int main(int argc, char **argv)
{
   
    
    Parse_options myoptions(argc, argv);

    if (myoptions.parse())
    {
        std::cout << "FdAIL:" << std::endl;
        std::cout << myoptions.helpText() << std::endl;
        return(0);
    }
    if (myoptions.help())
    {
        std::cout << myoptions.helpText() << std::endl;
        return (0);
    }

    //std::cout << 
    //return (0);
    Parse_config nbserver(myoptions.get_path());
    if (!nbserver.parse())
    {
        std::cout << "\nError en config:" << std::endl;
        return(0);
    }
        
    //return (0);
    
    std::cout << "Servidores:" << nbserver.get_nb_servers() << std::endl;
    //nbserver.show_config();
    //return (0);
    int c = 0;
    while (c < nbserver.get_nb_servers())
     {   
        std::cout << "\nServidor:" << c << std::endl;
        nbserver.show_config_one(c);
        c++;
     }

    //nbserver._configuration.at(1).show_ports();

    log.print(INFO,"KKK",RED,true);
    log.print(DEBUG,"KKK",BLUE,true);
    
    
    /*
    std::cout << "\nServidor:" << 2 << std::endl;
    nbserver.show_config_one(1);
    */
    
    //return(0);
    
    /*
    Config cfg(argv[1]);
    cfg.show_config();
    cfg.show_ports();
    std::cout << "workers:" << cfg.get_workers() << std::endl;
    std::cout << "CGI:" << std::endl;
    cfg.show_cgi();
    std::cout << "ERROR_PAGES:" << std::endl;
    cfg.show_error_pages();
    return 0;
    */



   /*
    int opt;
    char role = 's';
    while ((opt = getopt(argc, argv, "cs")) != -1){
        switch (opt)
        {
        case 'c':
            role = 'c';
            break;
        case 's':
            role = 's';
            break;
        default:
            printf("usage: %s [cs]\n", argv[0]);
            exit(1);
        }
    }
    if (role == 's'){
    */

    ///Using select
    /*
    {
        struct timeval tv;
        fd_set  master;     // master file descriptor list
        fd_set read_fds;    // temp file descriptor list for select()
        int fdmax;        // maximum file descriptor number
        int newfd;   
        int fd_listen;
        int nbytes;

        tv.tv_sec = 2;
        tv.tv_usec = 500000;

        SimpleSocket mysocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        int fd = mysocket.get_sock();
        
        FD_ZERO(&master);       // clear all
        FD_ZERO(&read_fds);     // clear all


        int nb_server = 0;
        int nb_port = 0;
        //creo los listeners 
        SimpleServer myserver1(AF_INET, SOCK_STREAM, IPPROTO_TCP,  nbserver._configuration.at(nb_server).port(nb_port), INADDR_ANY);
       

        std::cout << " socket = " << fd_listen << std::endl;
        std::cout << " socket = " << myserver1.get_socket() << std::endl; ///este
        fd_listen = myserver1.get_socket();

        //los uno al select add the listener to the master set
        FD_SET(fd_listen, &master);
        
        //CONTROLO EL MAX FD keep track of the biggest file descriptor
        fdmax=fd_listen; // so far, it's this one

    }
    */
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

        

        std::cout << "Starting Server" << std::endl;
        SimpleServer myserver(AF_INET, SOCK_STREAM, IPPROTO_TCP,  85, INADDR_ANY);
        myserver.launch();
        std::cout << "Ending" << std::endl;
        
    return (0);
}