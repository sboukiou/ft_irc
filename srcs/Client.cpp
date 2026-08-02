#include "../include/Client.hpp"

Client::Client():_fd(-1), disconnected(false) , registered(false), authenticated(false) {}

Client::Client(int fd):_fd(fd), disconnected(false) , registered(false), authenticated(false) {}

Client::Client(const Client& other){
    *this = other;
}
void Client::setRealname(std::string rName){
    realname = rName;
}
std::string Client::getRealkname(){
    return realname;
}
void Client::setDisconnected(bool status)
{
    disconnected = status;
}

void Client::setAuthenticated(bool status){
    authenticated = status;
}
bool Client::getAuthenticated(){
    return authenticated;
}
void Client::setRegistered(bool status){
    registered = status;
}
bool Client::getRegistered(){
    return registered;
}
void Client::addInvitedChannel(Channel *channel)
{
    invitedChannels.insert(channel);
}
void Client::removeInvitedChannel(Channel *channel)
{
    invitedChannels.erase(channel);
}
bool Client::isInvitedToChannel(Channel *channel)
{
    return invitedChannels.count(channel) > 0;
}

bool Client::getDisconnected()
{
    return disconnected;
}
void Client::setNickname(std::string nName)
{   
    nickname = nName;
}

void Client::appendChannels(Channel* channel){
    if (channels.count(channel))
        return ;
    channels.insert(channel);
}
void Client::removeChannel(Channel *channel)
{
    channels.erase(channel);
}
std::set<Channel*>& Client::getChannels(){
    return channels;
}

void Client::appendToResponse(std::string res)
{
    response.append(res);
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
std::string& Client::getResponse()
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