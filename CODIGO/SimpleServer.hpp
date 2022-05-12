#ifndef SIMPLESERVER_HPP
#define SIMPLESERVER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SimpleSocket.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
//#include <sys/epoll.h>

#define exit_if(r, ...)                                                                          \
    if (r) {                                                                                     \
        printf(__VA_ARGS__);                                                                     \
        printf("%s:%d error no: %d error msg %s\n", __FILE__, __LINE__, errno, strerror(errno)); \
        exit(1);                                                                                 \
    }

namespace WEB
{
    class SimpleServer
    {
        private:
            //WEB::SimpleSocket   webSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            int                 _port;
            u_long              _interface;
            struct sockaddr_in  _address;
            int                 _addresslen;
            int                 _server_socket;
            int                 _backlog;           //maximun of number of pending connetions before are refused
            int                 _listening;
            char                _buffer[30000] = {0}; 
            int                 _epfd; //epoll
       public:
            // Constructor
            SimpleServer(int domain, int service, int protocol, int port, u_long interface);           
            
            void start_listening(void);
            void launch(void);

            void test_binding(int item_to_test);
            void test_listening(int item_to_test);
            void setNonBlockingFD(int fd);
            //getter
            int get_port(){return (_port);};
            u_long get_interface(){return(_interface);};
            int get_socket(){return (_server_socket);};
    };
}
#endif
