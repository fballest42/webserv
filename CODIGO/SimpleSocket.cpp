#include "SimpleSocket.hpp"


//Default constructor
WEB::SimpleSocket::SimpleSocket(int domain, int service, int protocol)
{
    //Stablish connection
    _sock  = socket(domain, service, protocol);
    //Test connection
    test_socket(_sock);
}

//Test connection virtual function
void WEB::SimpleSocket::test_socket(int item_to_test)
{
    //Confirm that te socket or connection has been properly established
    if (item_to_test < 0)
    {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    } else {
        std::cout << "socket() " << item_to_test << " is OK!" << std::endl;
    }
}

