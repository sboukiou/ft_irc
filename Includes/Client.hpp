#ifndef Client_hpp
#define Client_hpp

#include <string>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <poll.h>
#include "Client.hpp"
#include <netinet/in.h>

class Client{
    private:
        int _fd;
        std::string buffer;
        std::string nickname;
        std::string username;
        std::string response;

        bool disconnected;

    public:
        Client();
        Client(int fd);
        Client(const Client& other);
        Client& operator=(const Client& other);
        ~Client();

        void setNickname(std::string nName);
        void setResponse(std::string res);
        void setUsername(std::string uName);
        std::string getNickname();
        std::string getUsername();
        std::string getResponse();
        void setDisconnected(bool status);
        bool getDisconnected();
        void appendBuffer(std::string buf);
        std::string& getBuffer() ;
        int getFd() const;
};


#endif