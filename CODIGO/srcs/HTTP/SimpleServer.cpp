#include "SimpleServer.hpp"
//#include "Request.hpp"
extern Logger log;
extern Error_page err_page;
//Constructor 
SimpleServer::SimpleServer(int domain, int service, int protocol, int port, u_long interface)
{
     //Logger log1;
     //log1.print(DEBUG,"KKK",BLUE,true);
     //webSocket(domain, service, protocol);
     //webSocket = new  WEB::SimpleSocket::SimpleSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
     
     _port = port;
     _interface = interface;
     SimpleSocket webSocket(domain, service, protocol);
     _server_socket = webSocket.get_sock();
     
     // Define Address structure
    /**************************************************************************/
    /* struct SOCKADDR_IN{short sin_family; u_short sin_port;\                */
    /*        struct in_addr sin_addr; char sin_zero[0]; };                   */
    /*                                                                        */
    /* struct in_addr{unsigned long s_addr; }                                 */
    /*                                                                        */
    /* # sin_family : Specifies the adress family, usualy the constant \      */
    /*         "AF_INET"                                                      */
    /* # sin_port   : Specifies the port number and must to be used with      */
    /*           htons() function which converts the "host byte order" to     */
    /*           "network byte order" so it can be transmited and routed      */
    /*           properly when opening the socket connection. the reason for  */
    /*           this is that computers and network protocols order their     */
    /*           bytes in a non-compatible fashion, each the opposite of      */
    /*           the other                                                    */
    /* # sin_addr   : holds the IP adress returned by inet_addr() to be used  */
    /*           in the socket connection                                     */
    /* # siz_zero   : usualy set to 0                                         */
    /**************************************************************************/
     _address.sin_family = domain; 
     _address.sin_port = htons(port);                  /*htons converts a short to a network representation*/
     _address.sin_addr.s_addr = htonl(interface);      /*htonl converts a long to a network representation*/
     memset(_address.sin_zero, '\0', sizeof _address.sin_zero);
     _addresslen = sizeof(_address);

     int connect_to_network;
     connect_to_network = bind(_server_socket, (struct sockaddr *)&_address, _addresslen);
     test_binding(connect_to_network);

    _listening = listen(_server_socket, _backlog);
    test_listening(_listening);
    log.print(INFO,"Server listening on port:" + std::to_string(_port),GREEN,true);
}

void SimpleServer::launch(void)
{
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
     int fdmax;        // maximum file descriptor number
     int fdmin;        // maximum file descriptor number
     int client_socket;
     int nbytes;
     char *hello = "Hello from server";
     //setNonBlockingFD(_server_socket);//Not blocking socket of server
     
    FD_ZERO(&master);    // clear the master sets
    FD_ZERO(&read_fds); // clear the temp sets
    // add the listener to the master set
    FD_SET(_server_socket, &master);
    // keep track of the biggest number file descriptor
    fdmax = _server_socket; // so far, it's this one
    fdmin = _server_socket; // so far, it's this one
    std::cout << "fdmax:" << fdmax << std::endl;
    int i, j, rv;
    while(1)
    {
        read_fds = master; // copy master to read_fs
        log.print(INFO,"waiting for connections",GREEN,true);
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }
        // run through the existing connections looking for data to read
        for(i = 0 ; i <= fdmax; i++)
        {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == _server_socket) { // handle new connections
                    if ((client_socket = accept(_server_socket, (struct sockaddr *)&_address, (socklen_t*)&_addresslen))<0)
                    {
                        perror("Error accepting connection");
                        exit(EXIT_FAILURE);
                    } 
                    else 
                    {
                        std::cout << "new socket" << client_socket << std::endl;
                        FD_SET(client_socket, &master); // add to master
                        if (client_socket > fdmax) {    // keep track of the max sockets
                            fdmax = client_socket;
                        }
                        sockaddr_in client_addr;
                        socklen_t alen = sizeof(client_addr);
                        getpeername(_server_socket,(struct sockaddr *)&client_addr, &alen);
                        std::string a(inet_ntoa(client_addr.sin_addr));
                        std::string b= std::to_string(ntohs(client_addr.sin_port));
                        std::string msg = "New conection on:" + a + ";"+b;
                        log.print(INFO, msg, GREEN, true);

                    }
                } 
                else 
                {   // handle data from a client
                    if ((nbytes = recv(i, _buffer, sizeof _buffer, 0)) <= 0) 
                    {   // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selected server: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        log.print(INFO,"Connection closed",GREEN,true);
                        FD_CLR(i, &master); // remove from master set
                    } 
                    else
                    {   // we got some data from a client
                        std::cout << _buffer << std::endl;
                        char *hello = "Hello from the server";//IMPORTANT! WE WILL GET TO IT
                        //write(i , hello , strlen(hello));
                        //std::string m_body = err_page.get_error_page(505);
                        //write(i, m_body.c_str(), m_body.length());
                        //send(i, m_body.c_str(), m_body.length(), 0);
                        log.print(INFO,"Connection closed",GREEN,true);
                        
                        for(j = 0; j <= fdmax; j++) {
                            // send to everyone!
                            std::cout << "sending" << std::endl;
                            if (FD_ISSET(j, &master)) {
                                // except the listener and ourselves
                                if (j != _server_socket && j != i) {
                                    //if (send(j, _buffer, nbytes, 0) == -1) {
                                    if (write(i , hello , strlen(hello)) == -1) {
                                        perror("send");
                                    }
                                }
                            }
                        }
                        
                    }
                } // END handle data from client

            //log.print(INFO,"Server waiting for new connetions ---",RED,true);
            //write(_server_socket, "GET / HTTP/1.1\r\nHost: www.42.fr\r\n\r\n", strlen("GET / HTTP/1.1\r\nHost: www.42.fr\r\n\r\n"));
        
            // accepter();


            //log.print(INFO,"New connection on", GREEN);
            
            //setNonBlockingFD(_server_socket);//Not blocking socket of server
            //setNonBlockingFD(client_socket);
            // get info from client

            //std::cout << "Client connected from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
            
        
            /*
            Request newrequest;
            std::string  req(_buffer);
            int status = 0;
            status = newrequest.parse(req);
            if (status > 0)
                {
                    std::cout << "ERROR" << status << std::endl;
                            // responde
                    std::cout << "----- response error sent -------" << std::endl;
                    //std::string m_body = err.get_error_page(status);
                    //write(client_socket, m_body.c_str(), m_body.length());
                }
            else {
                
                std::cout << "--" <<  newrequest.get_start_timer_in_sec() << "--" << std::endl;
                std::cout << "-- Este es el metodo: " << newrequest.get_method()   << std::endl; 
                std::cout << "-- Estos es el target: " << newrequest.get_target() << std::endl; 
                std::cout << "-- Estos es el protocolo: " << newrequest.get_protocol() << std::endl; 
                std::cout << "-- Estos son los headers" << std::endl; 
                
                newrequest.get_headers();

                // responde
                //std::cout << "----- response sent -------" << std::endl;
                log.print(INFO,">>> Response sent",GREEN,true);
                Response newresponse;
                newresponse.sendResponse(client_socket);
            }
            */
            }
        } // END got new incoming connection
    } // END looping through file descriptors
}

//Test connection virtual function
void SimpleServer::test_binding(int item_to_test)
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
void SimpleServer::test_listening(int item_to_test)
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

void SimpleServer::setNonBlockingFD(int fd) {
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
