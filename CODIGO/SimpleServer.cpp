#include "SimpleServer.hpp"

//Constructor 
WEB::SimpleServer::SimpleServer(int domain, int service, int protocol, int port, u_long interface)
{
     //webSocket(domain, service, protocol);
     //webSocket = new  WEB::SimpleSocket::SimpleSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
     _port = port;
     _interface = interface;
     WEB::SimpleSocket webSocket(domain, service, protocol);
     // Define Address structure
     _address.sin_family = domain; 
     _address.sin_port = htons(port);                  /*htons converts a short to a network representation*/
     _address.sin_addr.s_addr = htonl(interface);      /*htonl converts a long to a network representation*/
     memset(_address.sin_zero, '\0', sizeof _address.sin_zero);
     _server_socket = webSocket.get_sock();
     _addresslen = sizeof(_address);

     int connect_to_network;
     connect_to_network = bind(_server_socket, (struct sockaddr *)&_address, _addresslen);
     //setNonBlockingFD(_server_socket);
     test_binding(connect_to_network);
}

void WEB::SimpleServer::start_listening()
{
    //Start listening on the network
    //setNonBlockingFD(_server_socket);//Not blocking socket of server
    _listening = listen(_server_socket, _backlog);
    test_listening(_listening);
    //setNonBlockingFD(_server_socket);//Not blocking socket of server
    //HERE IS WHERE PUT NOTBLOCKING FEAUTURE
    //setNonBlock(_server_socket);//Not blocking 

    //epoll
    //_epfd = epoll_create(1);
    // añado el socket al poll
    // epoll_ctl_add(epfd, _server_socket, EPOLLIN | EPOLLOUT | EPOLLET);
    // f: struct epoll_event ev;
    //    ev.events = events;
    //    ev.data.fd = fd;
    // if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1){
    //    perror("epoll_ctl(n)\n");
    //    exit(1);
    //}
}

void WEB::SimpleServer::launch(void)
{
     int client_socket;
     long valread;
     char *hello = "Hello from server";
     //setNonBlockingFD(_server_socket);//Not blocking socket of server
     while(1)
     {
          std::cout << "---- Waiting for new connetions ---" << std::endl;
           // accepter();
          if ((client_socket = accept(_server_socket, (struct sockaddr *)&_address, (socklen_t*)&_addresslen))<0)
          {
               perror("Error accepting connection");
               exit(EXIT_FAILURE);
          };
          
          //setNonBlockingFD(_server_socket);//Not blocking socket of server
          //setNonBlockingFD(client_socket);
          // get info from client
          sockaddr_in client_addr;
          socklen_t alen = sizeof(client_addr);
          getpeername(_server_socket,(struct sockaddr *)&client_addr, &alen);
          std::cout << "Client connected from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
          
          //HERE IS WHERE PUT ECHOHANDLER
          //new EchoHandler(new_socket, client_addr);
          
          // handle
          valread = read(client_socket, _buffer, 30000);
          std::cout << _buffer << std::endl;
          // responde
          std::cout << "----- response sent -------" << std::endl;
          write(client_socket, hello, strlen(hello));
          close(client_socket);
     }

}

//Test connection virtual function
void WEB::SimpleServer::test_binding(int item_to_test)
{
    //Confirm that te socket or connection has been properly established
    if (item_to_test < 0)
    {
        perror("Failed to bind server socket");
        exit(EXIT_FAILURE);
    } else {
        std::cout << "Connect to network bind() " << item_to_test << " is OK! in port:" << this->get_port() << std::endl;
    }
}

//Test connection virtual function
void WEB::SimpleServer::test_listening(int item_to_test)
{
    //Confirm that te socket or connection has been properly established
    if (item_to_test < 0)
    {
        perror("Failed to listen on server socket");
        exit(EXIT_FAILURE);
    } else {
        std::cout << "Server listening " << item_to_test << " in port:" << this->get_port() << std::endl;
    }
}

void WEB::SimpleServer::setNonBlockingFD(int fd) {
    //int flags = fcntl(fd, F_GETFL, 0);
    //exit_if(flags < 0, "fcntl failed");
    //int r = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    int r = fcntl(fd, F_SETFL, O_NONBLOCK);
    exit_if(r < 0, "fcntl failed");
}

/*
void WEB::TestServer::accepter()
{
    struct sockaddr_in address = get_socket()-> get_address(); 
    int addrlen = sizeof(address);
    new_socket = accept(get_socket()->get_sock(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
    read(new_socket, buffer, 30000);
}

void WEB::TestServer::handler()
{
    std::cout << buffer << std::endl;

}

void WEB::TestServer::responder()
{
    char *hello = "Hello from server";
    write(new_socket, hello, strlen(hello));
    close(new_socket);
}

void WEB::TestServer::launch()
{
    while (true)
    {
        std::cout << "===== waiting =====" << std::endl;
        accepter();
        handler();
        responder();
    }
    std::cout << "===== DONE =====" << std::endl;
}
*/
//      accepter();
//        handler();
//        responder();
//        launch()
