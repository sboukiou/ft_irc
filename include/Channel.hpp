#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <set>

class Client;

class Channel 
{
private:
    std::string       _name;
    std::set<Client*> _members;
    std::set<Client*> _operators;
    std::string _topic;
    bool        _topicRestricted;

public:
    Channel();
    Channel(const std::string& name);
    ~Channel();

    const std::string& getName() const;

    void addClient(Client* client);
    void removeClient(Client* client);
    bool isMember(Client* client) const;
    bool isEmpty() const;
    size_t getMemberCount() const;

    void addOperator(Client* client);
    bool isOperator(Client* client) const;

    void setTopic(const std::string& topic);
    bool hasTopic() const;
    const std::string& getTopic() const;

    bool isTopicRestricted() const;
    void setTopicRestricted(bool value);
};

#endif