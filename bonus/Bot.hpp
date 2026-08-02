#ifndef BOT_HPP
#define BOT_HPP

#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include "../include/macros.hpp"
#include <ctime>

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
    bool receive();
    bool extractLine(std::string& line);

    void authenticate();
    void parseMessage(const std::string &msg);
    void handlePing(const std::string &msg);
    void handlePrivmsg(const std::string &msg);
    void sendRaw(const std::string &raw);
    void execCmd(const std::string &target, const std::string &cmd);

};

#endif