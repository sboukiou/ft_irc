#ifndef Server_hpp
#define Server_hpp

#include <string>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <poll.h>
#include "Client.hpp"
#include <netinet/in.h>

class Server
{
    private:
        int _socket;
        int _port;
        std::string _password;

    public:
        Server(int port, std::string password);
        Server();
        Server(const Server& other);
        Server& operator=(const Server& other);
        ~Server();

        void initSocket();

};



#endif