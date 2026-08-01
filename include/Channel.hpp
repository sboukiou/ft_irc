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
    std::string       _pass;
    std::string _topic;

    bool        _inviteOnly;
    bool        _topicRestricted;
    bool        _channelPass;
    bool        _userLimit;

    size_t         _nMembers;

public:
    Channel();
    Channel(const std::string& name);
    Channel(const std::string& name, std::string pass);
    ~Channel();

    std::set<Client*>& getMembers();
    const std::string& getName() const;
    void setInviteOnly(bool status);
    void setChannelPass(bool status);
    void setUserLimit(bool status);
    void setNMembers(size_t nMembers);
    void setPass(std::string pass);
    bool getInviteOnly();
    bool getChannelPass();
    bool getUserLimit();
    size_t getNMembers();
    std::string getPass();
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