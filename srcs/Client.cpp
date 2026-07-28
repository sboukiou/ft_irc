#include "Client.hpp"

Client::Client():_fd(-1), is_complete(false) {}

Client::Client(int fd):_fd(fd), is_complete(false){}

Client::Client(const Client& other){
    *this = other;
}

Client& Client::operator=(const Client& other){
    (void) other;
    return *this;
}

Client::~Client() {}

void Client::setIsCoplete(bool status)
{
    is_complete = status;
}
bool Client::getIsComplete() const
{
    return is_complete;
}
int Client::getFd() const
{
    return _fd;
}
void Client::appendBuffer(std::string buf)
{
    buffer.append(buf);
}
std::string Client::getBuffer() const
{
    return buffer;
}