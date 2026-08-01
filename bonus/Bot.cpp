#include "Bot.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


Bot::Bot() {}

Bot::Bot(int port, std::string host, std::string pass, std::string nick, std::string user, std::string name) : 
_port(port), _host(host), _pass(pass), _nick(nick), _user(user), _name(name){}

Bot::Bot(const Bot &other) :
_port(other._port), _host(other._host), _pass(other._pass), _nick(other._nick) {}

Bot::~Bot() {}

bool Bot::connectServer()
{
    _socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socketFd < 0)
        throw std::runtime_error("Error: Socket failed");
    sockaddr_in botInfo = {};
    botInfo.sin_family = AF_INET;
    botInfo.sin_port = htons(_port);
    if (inet_pton(AF_INET, _host.c_str(), &botInfo.sin_addr) <= 0)
        throw std::runtime_error("Invalid address");

    if (connect(_socketFd, (sockaddr *)&botInfo, sizeof(botInfo)) < 0)
        throw std::runtime_error("Error: connect failed");

    return true;
}

void Bot::sendRaw(const std::string &msg)
{
    if(send(_socketFd, msg.c_str(), msg.size(), 0) < 0)
        throw std::runtime_error("Error: send failed");
}

void Bot::authenticate()
{
    sendRaw("PASS " + _pass + "\r\n");
    sendRaw("NICK " + _nick + "\r\n");
    sendRaw("USER " + _user + " 0 * :" + _name + "\r\n");
}
