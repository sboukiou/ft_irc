#ifndef BOT_HPP
#define BOT_HPP

#include <iostream>

class Bot
{
private:
    int _socketFd;
    int _port;

    std::string _host;
    std::string _pass;
    std::string _nick;
    std::string _user;
    std::string _name;


public:
    Bot();
    Bot(int port, std::string host, std::string pass, std::string nick, std::string _user, std::string name);
    Bot(const Bot &other);
    Bot &operator=(const Bot &other);
    ~Bot();

    bool connectServer();
    void authenticate();
    void receive();
    void run();
    void parseMessage(const std::string &msg);
    void sendRaw(const std::string &raw);

};

#endif