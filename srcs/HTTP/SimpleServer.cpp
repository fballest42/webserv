#include "SimpleServer.hpp"

extern Logger log;
extern Error_page err_page;

void SimpleServer::server_port_launch(void)
{
	//load arguments
	char                _buffer[1024];
	fd_set master;    // master file descriptor list
	fd_set read_fds;  // temp file descriptor list for select()
	int fdmax;        // maximum file descriptor number
	int fdmin;        // maximum file descriptor number
	int client_socket;
	int nbytes;
	std::string         request_string;

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
						log.print(INFO, msg, GREEN, true);
					}
				} 
				else 
				{   // handle data from a old client                  
					request_string.clear();
					while ((nbytes = recv(i, _buffer, (sizeof _buffer) - 1, 0)) > 0)  /* Hay un error en la lectura. Posiblemente el cliente ha cerrado la conexión. Hacer aquí el tratamiento. En el ejemplo, se cierra el socket y se elimina del array de socketCliente[] */
					{
						// got error or connection closed by client
						// we got some data from a client  
						// Checking the request
						_buffer[nbytes] = '\0';
						request_string += _buffer;
					}
					if (nbytes == -1) // EAGAIN)
					{				   
						//CHECK SI EN EL PUERTO ESTA EL SERVER NAME Y SI ESO CARGAR CONFIG 
						std::string host_name;
						int result = 0;
						if (request_string.find("host") != std::string::npos || request_string.find("Host") != std::string::npos)
						{
							std::string str;
							if (request_string.find("Host") != std::string::npos ){
								str = (request_string.substr(request_string.find("Host"), std::string::npos));
								str = str.substr(0, str.find("\n"));
									if( str[str.size()-1] == '\n')
									str.pop_back();
								if( str[str.size()-1] == '\r')
									str.pop_back();
								if( str[str.size()-1] == '\n')
									str.pop_back();
								str = ft_clean_token(get_token(str,':',1));
								if (str == "127.0.0.1")
									str = "localhost";
							}
							else {
								str = (request_string.substr(request_string.find("host"), std::string::npos));
								str = str.substr(0, str.find("\n"));
								if( str[str.size()-1] == '\n')
									str.pop_back();
								if( str[str.size()-1] == '\r')
									str.pop_back();
								if( str[str.size()-1] == '\n')
									str.pop_back();
								str = ft_clean_token(get_token(str,':',2));
							}
							host_name = str;
							if (_myconfig.find(host_name)==_myconfig.end()) 
								result = 400;
						}
						else{
							result = 400;
						}
						Request newrequest;
						if (result != 400)
						{
							newrequest.set_request(_myconfig.at(host_name));
							result = newrequest.parse(request_string);  //compruebo request result = error
						}
						log.print(INFO,"<< [method: " + newrequest.get_method() + "] [target: "+ newrequest.get_target() +"] [server: " + std::to_string(newrequest._config._nb) + "] [location: " + newrequest.get_actualLocation() +"] + ret:" + std::to_string(result),YELLOW,true);
						if (FD_ISSET(i, &master)) { 
							Response newresponse(newrequest, result);
							if (newresponse.sendResponse(i) == -1) {
								perror("send");
							}  
						}
					}          
					close(i); // bye!
					log.print(INFO,"Connection closed",GREEN,true);
					FD_CLR(i, &master); // remove socket from master set
				} // END handle data from client
			}
		} // END got new incoming connection
	} // END looping through file descriptors
}

//Constructor 
SimpleServer::SimpleServer(int domain, int service, int protocol, int port, std::map < std::string, Config > mconfig, u_long interface): _port(port), _myconfig(mconfig)
{
		log.print(INFO,"Starting Port: "+ std::to_string(_port) + " in Server ",GREEN,true);
		_interface = interface;
		SimpleSocket webSocket(domain, service, protocol);
		_server_socket = webSocket.get_sock();
		_address.sin_family = domain; 
		_address.sin_port = htons(_port);        /*htons converts a short to a network representation*/
		_address.sin_addr.s_addr = htonl(interface);      /*htonl converts a long to a network representation*/
		memset(_address.sin_zero, '\0', sizeof _address.sin_zero);
		_addresslen = sizeof(_address);
		int connect_to_network;
		connect_to_network = bind(_server_socket, (struct sockaddr *)&_address, _addresslen);
		test_binding(connect_to_network);
		int listening = listen(_server_socket, _backlog);
		test_listening(listening);
		log.print(INFO,"Server  on port:" + std::to_string(_port),BLUE,true);
		server_port_launch();
		usleep(500);
	return;
	//**************************-------------------_****************///

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
 
}

//Test connection virtual function
void SimpleServer::test_binding(int item_to_test)
{
	//Confirm that te socket or connection has been properly established
	if (item_to_test < 0)
	{
		log.print(INFO,"Failed to bind server socket",RED,true);
		exit(EXIT_FAILURE);
	} 
}

//Test connection virtual function
void SimpleServer::test_listening(int item_to_test)
{
	//Confirm that te socket or connection has been properly established
	if (item_to_test < 0)
	{
		log.print(INFO,"Failed to listen on server socket",RED,true);
		exit(EXIT_FAILURE);
	} 
}

void SimpleServer::setNonBlockingFD(int fd) {
	int r = fcntl(fd, F_SETFL, O_NONBLOCK);
	if (r < 0)
	{
		log.print(INFO,"fcntl failed",RED,true);
		exit(EXIT_FAILURE);
	}
}
