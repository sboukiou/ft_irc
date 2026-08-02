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
    std::string _inputBuffer;


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
    void handlePing(const std::string &msg);
    void handlePrivmsg(const std::string &msg);
    void handleMessage(const std::string &msg);
    void sendRaw(const std::string &raw);
    bool extractLine(std::string& line);
    void execCmd(const std::string &target, const std::string &cmd);

};

#endif