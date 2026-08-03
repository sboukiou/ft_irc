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

class Channel;

class Client{
    private:
        int _fd;
        std::string buffer;
        std::string nickname;
        std::string username;
        std::string realname;
        std::string response;
        std::set<Channel *> channels;
        std::set<Channel *> invitedChannels;

        bool disconnected;
        bool registered;
        bool authenticated;

    public:
        Client();
        Client(int fd);
        Client(const Client& other);
        Client& operator=(const Client& other);
        ~Client();

        void removeChannel(Channel *channel);
        void addInvitedChannel(Channel *channel);
        void removeInvitedChannel(Channel *channel);
        bool isInvitedToChannel(Channel *channel);
        void appendChannels(Channel* channel);
        std::set<Channel*>& getChannels();
        void setNickName(std::string nName);
        void setRealName(std::string rName);
        void appendToResponse(std::string res);
        void setUserName(std::string uName);
        std::string getNickName();
        std::string getRealName();
        std::string getUserName();
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
