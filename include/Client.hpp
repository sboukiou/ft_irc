#ifndef Client_hpp
#define Client_hpp

#include <string>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <poll.h>
#include <set>
#include <netinet/in.h>
#include "Channel.hpp"

class Client{
    private:
        int _fd;
        std::string buffer;
        std::string nickname;
        std::string username;
        std::string realname;
        std::string response;
        std::set<Channel *> channels;

        bool disconnected;
        bool registered;
        bool authenticated;
        bool isOperator;

    public:
        Client();
        Client(int fd);
        Client(const Client& other);
        Client& operator=(const Client& other);
        ~Client();

        void appendChannels(Channel* channel);
        std::set<Channel*>& getChannels();
        void setIsOperator(bool status);
        bool getIsOperator();
        void setNickname(std::string nName);
        void setRealname(std::string rName);
        void appendToResponse(std::string res);
        void setUsername(std::string uName);
        std::string getNickname();
        std::string getRealkname();
        std::string getUsername();
        std::string& getResponse();
        void setDisconnected(bool status);
        bool getDisconnected();
        void setAuthenticated(bool status);
        bool getAuthenticated();
        void setRegistered(bool status);
        bool getRegistered();
        void appendBuffer(std::string buf);
        std::string& getBuffer() ;
        int getFd() const;
};


#endif