#ifndef SIMPLESOCKET_HPP
#define SIMPLESOCKET_HPP

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>



namespace WEB
{
    class SimpleSocket
    {
        private:
            // Once instantiated, sock contains the address of a socket object in the network
            // This should a not negative number
            int _sock;
        public:
            //Constructor
            SimpleSocket(int domain, int service, int protocol);
            
            //Getters
            int get_sock(){ return _sock;};

            void test_socket(int item_to_test);

    };
} // namespace web

#endif