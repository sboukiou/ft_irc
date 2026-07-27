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

};

#endif