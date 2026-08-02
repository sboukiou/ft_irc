#include "../include/Channel.hpp"

Channel::Channel() : _topicRestricted(false) {}

Channel::Channel(const std::string &name) : _name(name), _inviteOnly(false), _topicRestricted(false), _channelPass(false), _userLimit(false) {}
Channel::Channel(const std::string &name, std::string pass) : _name(name), _pass(pass), _inviteOnly(false), _topicRestricted(false), _channelPass(true), _userLimit(false) {}

Channel::~Channel() {}

const std::string &Channel::getName() const
{
    return _name;
}

void Channel::addClient(Client *client)
{
    _members.insert(client);
}

void Channel::setInviteOnly(bool status){
    _inviteOnly = status;
}

void Channel::setChannelPass(bool status){
    _channelPass = status;
}
std::set<Client*>& Channel::getMembers(){
    return _members;
}

void Channel::setUserLimit(bool status){
    _userLimit = status;
}

void Channel::setNMembers(size_t nMembers){
    _nMembers = nMembers;
}

void Channel::setPass(std::string pass){
    _pass = pass;
}

bool Channel::getInviteOnly(){
    return _inviteOnly;
}

bool Channel::getChannelPass(){
    return _channelPass;
}

bool Channel::getUserLimit(){
    return _userLimit;
}

size_t Channel::getNMembers(){
    return _nMembers;
}

std::string Channel::getPass(){
    return _pass;
}

std::set<Client*>& Channel::getOperators(){
    return _operators;
}

void Channel::removeClient(Client *client)
{
    _members.erase(client);
    if (isOperator(client))
        _operators.erase(client);
}

bool Channel::isMember(Client *client) const
{
    return _members.find(client) != _members.end();
}

bool Channel::isEmpty() const
{
    return _members.empty();
}

Client* Channel::isMemberByName(std::string name)
{
    for (std::set<Client*>::iterator it = _members.begin(); it != _members.end(); it++)
    {
        if ((*it)->getNickName() == name)
            return *it;
    }
    return NULL;
}
void Channel::addOperator(Client* client) 
{
    if (isMember(client))
        _operators.insert(client);
}

bool Channel::isOperator(Client* client) const 
{
    return _operators.find(client) != _operators.end();
}

size_t Channel::getMemberCount() const
{
    return _members.size();
}

void Channel::setTopic(const std::string &topic)
{
    _topic = topic;
}

bool Channel::hasTopic() const
{
    return !_topic.empty();
}

const std::string &Channel::getTopic() const
{
    return _topic;
}

void Channel::setTopicRestricted(bool val)
{
    _topicRestricted = val;
}

bool Channel::isTopicRestricted() const
{
    return _topicRestricted;
}
