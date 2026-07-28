#include "../Includes/Channel.hpp"

Channel::Channel() : _topicRestricted(false) {}

Channel::Channel(const std::string &name) : _name(name), _topicRestricted(false) {}

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
