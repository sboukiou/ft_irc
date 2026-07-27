#ifndef Server_hpp
#define Server_hpp

#include <string>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include "Client.hpp"

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

};



#endif