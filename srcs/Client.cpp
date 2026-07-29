#include "Client.hpp"

Client::Client():_fd(-1), disconnected(false) {}

Client::Client(int fd):_fd(fd), disconnected(false){}

Client::Client(const Client& other){
    *this = other;
}

void Client::setDisconnected(bool status)
{
    disconnected = status;
}

bool Client::getDisconnected()
{
    return disconnected;
}
void Client::setNickname(std::string nName)
{   
    nickname = nName;
}
void Client::setResponse(std::string res)
{
    response = res;
}
void Client::setUsername(std::string uName)
{
    username = uName;
}
std::string Client::getNickname()
{
    return nickname;
}
std::string Client::getUsername()
{
    return username;
}
std::string Client::getResponse()
{
    return response;
}
Client& Client::operator=(const Client& other){
    (void) other;
    return *this;
}

Client::~Client() {
    if (_fd >= 0)
        close(_fd);
}

int Client::getFd() const
{
    return _fd;
}
void Client::appendBuffer(std::string buf)
{
    buffer.append(buf);
}
std::string& Client::getBuffer() 
{
    return buffer;
}