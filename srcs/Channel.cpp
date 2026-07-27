#include "../Includes/Channel.hpp"

Channel::Channel() {}

Channel::Channel(const std::string &name) : _name(name) {}

Channel::~Channel() {}

const std::string &Channel::getName() const
{
    return _name;
}

void Channel::addClient(Client *client)
{
    _members.insert(client);
}

void Channel::removeClient(Client *client)
{
    _members.erase(client);
}

bool Channel::isMember(Client *client) const
{
    return _members.find(client) != _members.end();
}

bool Channel::isEmpty() const
{
    return _members.empty();
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
