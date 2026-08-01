#ifndef CHANNELMANAGER_HPP
#define CHANNELMANAGER_HPP

#include <string>
#include <map>
#include "Channel.hpp"

class ChannelManager
{
private:
    std::map<std::string, Channel *> _channels;

public:
    ChannelManager();
    ~ChannelManager();

    Channel *getOrCreateChan(const std::string &name) ;
    Channel *getOrCreateChan(const std::string &name, std::string pass) ;
    Channel *find(const std::string &name) ;
    void removeIfEmpty(const std::string &name);
};

#endif