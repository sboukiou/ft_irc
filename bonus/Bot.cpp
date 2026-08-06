#include "Bot.hpp"

Bot::Bot() {}

Bot::Bot(int port, std::string host, std::string pass,
    std::string nick, std::string user, std::string name) : 
_port(port), _host(host), _pass(pass), 
_nick(nick), _user(user), _name(name){}

Bot::Bot(const Bot &other) :
_port(other._port), _host(other._host), _pass(other._pass), 
_nick(other._nick), _user(other._user), _name(other._name){}


Bot::~Bot() {}


bool Bot::connectServer()
{
    _socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socketFd < 0)
        throw std::runtime_error("Error: Socket failed");
    sockaddr_in botInfo;
    memset(&botInfo, 0, sizeof(botInfo));
    botInfo.sin_family = AF_INET;
    botInfo.sin_port = htons(_port);
    if (inet_pton(AF_INET, _host.c_str(), &botInfo.sin_addr) <= 0)
        throw std::runtime_error("Error: Invalid address");

    if (connect(_socketFd, (sockaddr *)&botInfo, sizeof(botInfo)) < 0)
        throw std::runtime_error("Error: Connect failed");

    return true;
}

bool Bot::extractLine(std::string& line)
{
    size_t pos = _inputBuffer.find('\n');
    if (pos == std::string::npos)
        return false;
    line = _inputBuffer.substr(0, pos);
    if (!line.empty() && line[line.size() - 1] == '\r')
        line.erase(line.size() - 1);
    _inputBuffer.erase(0, pos + 1);
    return true;
}

bool Bot::receive()
{
    char buffer[1024];
    ssize_t bytes = recv(_socketFd, buffer, sizeof(buffer), 0);
    if (bytes <= 0)
    {
        std::cout << "Disconnected: authentication failed or server closed the connection.\n";
        return false;
    }
    _inputBuffer.append(buffer, bytes);
    std::string line;
    while (extractLine(line))
    {
        handlePing(line);
        handlePrivmsg(line);
    }
    return true;
}

void Bot::sendRaw(const std::string &msg)
{
    if(send(_socketFd, msg.c_str(), msg.size(), 0) < 0)
        throw std::runtime_error("Error: send failed");
}

void Bot::authenticate()
{
    std::string passLine = "PASS " + _pass + "\r\n";
    std::string nickLine = "NICK " + _nick + "\r\n";
    std::string userLine = "USER " + _user + " 0 * :" + _name + "\r\n";

    sendRaw(passLine);
    sendRaw(nickLine);
    sendRaw(userLine);
}

void Bot::handlePing(const std::string &msg)
{
    size_t pos = msg.find("PING");
    if (pos == std::string::npos)
        return ;
    if (pos != 0)
        return ;
    sendRaw("PONG" + msg.substr(4) + "\r\n");
}

void Bot::execCmd(const std::string &target, const std::string &cmd)
{
    if (cmd == "!ping")
        sendRaw("PRIVMSG " + target + " :pong\r\n");
    else if (cmd == "!help")
        sendRaw("PRIVMSG " + target + " :Available commands: !help, !ping, !time\r\n");
    else if (cmd == "!time")
    {
        std::time_t now = std::time(0);
        std::string timeStr(std::ctime(&now));
        if (!timeStr.empty() && timeStr[timeStr.size()-1] == '\n')
            timeStr.erase(timeStr.size()-1);
        sendRaw("PRIVMSG " + target + " :" + timeStr + "\r\n");
    }
    else 
        sendRaw("PRIVMSG " + target + " :Unknown command. Type !help\r\n");
}

void Bot::handlePrivmsg(const std::string &msg)
{
    size_t pos = msg.find("PRIVMSG");
    if (pos == std::string::npos)
        return ;

    size_t start = msg.find(":");
    size_t end = msg.find("!");
    if (start == std::string::npos || end == std::string::npos)
        return;

    std::string sender = msg.substr(start + 1, end - start -1);
    size_t secondColon = msg.rfind(":");
    std::string messageCmd = msg.substr(secondColon + 1);
    size_t posSpace = messageCmd.find(" ");
    messageCmd = messageCmd.erase(posSpace);
    size_t chanStart = pos + 8;
    size_t chanEnd = msg.find(' ', chanStart);
    std::string target = msg.substr(chanStart, chanEnd - chanStart);
    if (target.empty() || target[0] != '#')
        target = sender;
    execCmd(target, messageCmd);
}

bool digits(const std::string &port)
{
    for (int i = 0; port[i]; i++)
    {
        if (!isdigit(port[i]))
            return false;
    }
    return true;
}