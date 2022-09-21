#include "SimpleSocket.hpp"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
extern Logger log;
//Default constructor
SimpleSocket::SimpleSocket(int domain, int service, int protocol)
{
    int opt = 1;
    /**************************************************************************/
    /* int socket(int domain, int type, int protocol);                        */
    /*                                                                        */
    /* crée un point de communication, et renvoie un descripteur.             */
    /* # domain   : AF_INET / AF_INET6                                        */
    /* # type     :                                                           */
    /*     - SOCKE_STREAM : reliable(fiable) byte stream(i.e TCP)             */
    /*     - SOCKE_DGRAM : unreliable datagrams(i.e UDP)                      */
    /* # protocol :                                                           */
    /*     - 0 : use default based on domain and type                         */
    /*     - IPROTO_TCP : TCP                                                 */
    /*     - IPROTO_UDP : UCP                                                 */
    /**************************************************************************/
    _sock  = socket(domain, service, protocol);
    //Test creation
    test_socket(_sock);
    // 
    if (setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
            throw std::string("SetSockopt Failed");
    }
    setNonBlockingFD(_sock);
}

//Test connection virtual function
void SimpleSocket::test_socket(int item_to_test)
{
    //Confirm that te socket or connection has been properly established
    if (item_to_test < 0)
    {
        
        log.print(INFO,"Failed to create socket",RED,true);
        exit(EXIT_FAILURE);
    }
}

void SimpleSocket::setNonBlockingFD(int socketfd) {
    int status = fcntl(socketfd, F_SETFL, O_NONBLOCK);
    if (status < 0)
    {
        log.print(INFO,"fcntl failed",RED,true);
        exit(EXIT_FAILURE);
    }
}

