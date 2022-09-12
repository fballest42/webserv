#include "SimpleServer.hpp"
//#include "Request.hpp"

extern Logger log;
extern Error_page err_page;

void *server_port_launch(void *arg)
{
    //load arguments
    int _server_socket = ((Server_port*)arg)->_server_socket;
    struct sockaddr_in  _address = ((Server_port*)arg)->_address;
    int                 _addresslen = ((Server_port*)arg)->_addresslen;
    char                _buffer[1025];
    Config              _config = ((Server_port*)arg)->_config;
    int                 _port =  ((Server_port*)arg)->_port;

    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number
    int fdmin;        // maximum file descriptor number
    int client_socket;
    int nbytes;

    std::string         request_string;

     
     //setNonBlockingFD(_server_socket);//Not blocking socket of server
     
    FD_ZERO(&master);    // clear the master sets
    FD_ZERO(&read_fds); // clear the temp sets
    // add the listener to the master set
    
    FD_SET(_server_socket, &master);
    // keep track of the biggest number file descriptor
    fdmax = _server_socket; // so far, it's this one
    fdmin = _server_socket; // so far, it's this one

    std::map< int , struct timespec> _start_time_connection;

    int i, j, rv;
    log.print(INFO,"Waiting for connections in Server " + std::to_string(_config._nb) + " in port: " + std::to_string(_port) ,GREEN,true);
    while(1)
    {
        //char _buffer[30000];
        read_fds = master; // copy master to read_fs
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {  // Select wait to something happens (new client connect, old client connect, close connected cliente)
            perror("select");
            exit(4);
        }
        // run through the existing connections looking for data to read
        for(i = 0 ; i <= fdmax; i++)
        {
            if (FD_ISSET(i, &read_fds)) { // we got one!! /* Un nuevo cliente solicita conexión. Aceptarla aquí. En el ejemplo, se acepta la conexión, se mete el descriptor en socketCliente[] y se envía al cliente su posición en el array como número de cliente. */
                if (i == _server_socket) { // handle new connections, it is a new connection
                    if ((client_socket = accept(_server_socket, (struct sockaddr *)&_address, (socklen_t*)&_addresslen))<0) 
                    {
                        perror("Error accepting connection");
                        exit(EXIT_FAILURE);
                    } 
                    else 
                    {
                        //std::cout << "new socket" << client_socket << std::endl;
                        FD_SET(client_socket, &master); // add new clsocket client to master
                        if (client_socket > fdmax) {    // keep track of the max sockets
                            fdmax = client_socket;
                        }
                        sockaddr_in client_addr;
                        socklen_t alen = sizeof(client_addr);
                        getpeername(_server_socket,(struct sockaddr *)&client_addr, &alen);
                        std::string a(inet_ntoa(client_addr.sin_addr));
                        std::string b= std::to_string(ntohs(client_addr.sin_port));
                        std::string msg = "New conection on:" + a + ";" + b;
                        //printf("selectserver: new connection from %s on socket %d\n", inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN), newfd);
                        log.print(INFO, msg, GREEN, true);

                        //--- Aqui empieza el tiempo de conexión
                        //std::map< int , struct timeval> 
                        // struct timespec start_time;
                        // // clock_gettime(CLOCK_REALTIME, &start_time);
                        // //gettimeofday(&start_time, NULL); //get time and store in start_timer
                        // _start_time_connection[client_socket] = start_time;
                        //log.print(INFO,"Set Time: " ,RED,true); 
                    }
                } 
                else 
                {   // handle data from a old client
                    //Check time of connection
                    // struct timespec current_time_connection;
                    // clock_gettime(CLOCK_REALTIME, &current_time_connection);
                    // if (time_diff2(&_start_time_connection[i], &current_time_connection) > 10.0)
                    // {
                    //     printf("loop Func  time spent: %0.8f sec\n", time_diff2(&_start_time_connection[i], &current_time_connection));
                    //     log.print(INFO,"Rise Time",RED,true); 
                    //     // close(i); // bye!
                    //     // log.print(INFO,"Connection closed",GREEN,true);
                    //     // FD_CLR(i, &master); // remove socket from master set
                    // }                    
                    request_string.clear();
                    while ((nbytes = recv(i, _buffer, (sizeof _buffer) - 1, 0)) > 0)  /* Hay un error en la lectura. Posiblemente el cliente ha cerrado la conexión. Hacer aquí el tratamiento. En el ejemplo, se cierra el socket y se elimina del array de socketCliente[] */
                    {
                        // got error or connection closed by client
                        // we got some data from a client  
                        // std::cout << _buffer << std::endl;
                        // Checking the request
                        _buffer[nbytes] = '\0';
                        request_string += _buffer;
//                        std::cout << "nbytes = " << nbytes << " sizeof buffer : " << (sizeof _buffer) - 1 << std::endl;
//                        std::cout << "requests_string.size = " << request_string.size() << " -- nbytes condition = " << (nbytes < (sizeof _buffer) -1) << std::endl;
                    }
                    if (nbytes == -1) // EAGAIN)
                    {
//                        std::cout << "nbytes = " << nbytes << " : errno = " << errno << std::endl;
//                        std::cout << "errno " << errno << std::endl;
                        Request newrequest(_config);
                        int result = newrequest.parse(request_string);  //compruebo request result = error
                        //newrequest.show_request_data();
                        log.print(INFO,"<< [method: " + newrequest.get_method() + "] [target: "+ newrequest.get_target() +"] [server: " + std::to_string(newrequest._config._nb) + "] [location: " + newrequest.get_actualLocation() +"] + ret:" + std::to_string(result),YELLOW,true);
                        if (FD_ISSET(i, &master)) { 
                            //log.print(INFO,"----------- STARTING RESPONSE -----------",YELLOW,true);
                            Response newresponse(newrequest, result);
                            if (newresponse.sendResponse(i) == -1) {
                                perror("send");
                            }
                            //log.print(INFO,"Response sent",GREEN,true);    
//                                close(i); // bye!
//                                log.print(INFO,"Connection closed",GREEN,true);
//                                FD_CLR(i, &master); // remove socket from master set    
                        }
                    }                      
//                    else {
//                        perror("recv");
//                    }
                    close(i); // bye!
                    log.print(INFO,"Connection closed",GREEN,true);
                    FD_CLR(i, &master); // remove socket from master set
                } // END handle data from client
            }
        } // END got new incoming connection
    } // END looping through file descriptors
}

//Constructor 
SimpleServer::SimpleServer(int domain, int service, int protocol, Config mconfig, u_long interface):_config(mconfig)
{
    
    int total_ports = _config.get_ports().size();
    
    _server_port.resize(total_ports);   
    
    for (int j = 0; j < total_ports; j++)
    {
        log.print(INFO,"Starting Port: "+ std::to_string(_config.port(j)) + " in Server " + std::to_string(_config._nb),GREEN,true);
        _server_port[j]._id = j;  
        _server_port[j]._port = _config.port(j);                       
        _server_port[j]._config = _config;
        _server_port[j]._interface = interface;
        SimpleSocket webSocket(domain, service, protocol);
        _server_port[j]._server_socket = webSocket.get_sock();
        _server_port[j]._address.sin_family = domain; 
        _server_port[j]._address.sin_port = htons(_config.port(j));        /*htons converts a short to a network representation*/
        _server_port[j]._address.sin_addr.s_addr = htonl(interface);      /*htonl converts a long to a network representation*/
        memset(_server_port[j]._address.sin_zero, '\0', sizeof _server_port[j]._address.sin_zero);
        _server_port[j]._addresslen = sizeof(_server_port[j]._address);
        
        int connect_to_network;
        connect_to_network = bind(_server_port[j]._server_socket, (struct sockaddr *)&_server_port[j]._address, _server_port[j]._addresslen);
        test_binding(connect_to_network);
        int listening = listen(_server_port[j]._server_socket, _server_port[j]._backlog);
        test_listening(listening);
        log.print(INFO,"Server "+ std::to_string(_config._nb) +" on port:" + std::to_string(_server_port[j]._port),BLUE,true);

        pthread_create(&_server_port[j]._thr,NULL, server_port_launch ,&_server_port[j]);
        usleep(500);
    }
    for (int j = 0; j < total_ports; j++)
    {
        pthread_join(_server_port[j]._thr,NULL);
    }

    return;
    //**************************-------------------_****************///

        _port = _config.port(0);
        //log.print(DEBUG,"port"+ std::to_string(_port),BLUE,true);
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
        _address.sin_port = htons(_port);                  /*htons converts a short to a network representation*/
        _address.sin_addr.s_addr = htonl(interface);      /*htonl converts a long to a network representation*/
        memset(_address.sin_zero, '\0', sizeof _address.sin_zero);
        _addresslen = sizeof(_address);

        int connect_to_network;
        connect_to_network = bind(_server_socket, (struct sockaddr *)&_address, _addresslen);
        test_binding(connect_to_network);

        _listening = listen(_server_socket, _backlog);
        test_listening(_listening);
        log.print(INFO,"Server "+ std::to_string(_config._nb) +" on port:" + std::to_string(_port),GREEN,true);
 
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
        //std::cout << "Connect to network bind() " << item_to_test << " is OK! in port:" << this->get_port() << std::endl;
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
        //std::cout << "Server listening " << item_to_test << " in port:" << this->get_port() << std::endl;
    }
}

void SimpleServer::setNonBlockingFD(int fd) {
    int r = fcntl(fd, F_SETFL, O_NONBLOCK);
    if (r < 0)
    {
        std::cout << "fcntl failed" << std::endl;
        exit(1);
    }
}