// g++ test.cpp SimpleSocket.cpp SimpleServer.cpp

#include "SimpleSocket.hpp"
#include "SimpleServer.hpp"
#include "utils/get_next_line.hpp"
#include "utils/config.hpp"

using namespace WEB;
int main(int argc, char **argv)
{
    int fd;
    
    //fd = open(argv[1], O_RDONLY);
    //if (fd < 0)
    //    return 1;
        //throw webserv_exception("could not open configuration file : %", 0, argv[1]);
    
    /*
    char *line;
    while(get_next_line(fd, &line))
        {std::cout << line << std::endl;}
    */
    Config cfg(argv[1]);

    cfg.show_config();
    cfg.show_ports();
    std::cout << "workers:" << cfg.get_workers() << std::endl;
 
    return 0;



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
        std::cout << "Starting Server" << std::endl;
        WEB::SimpleSocket mysocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        WEB::SimpleServer myserver(AF_INET, SOCK_STREAM, IPPROTO_TCP, 80, INADDR_ANY);
        myserver.start_listening();
        myserver.launch();
        std::cout << "Ending" << std::endl;
    } else {
        std::cout << "Starting Client" << std::endl;
        //cliente_run();
    }
    return (0);
}