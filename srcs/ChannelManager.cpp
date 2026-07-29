#include "../include/ChannelManager.hpp"

ChannelManager::ChannelManager() {}

ChannelManager::~ChannelManager()
{
    std::map<std::string, Channel*>::iterator it;
    for (it = _channels.begin(); it != _channels.end(); it++)
        delete it->second;
}

Channel *ChannelManager::getOrCreateChan(const std::string &name)
{
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end())
        return it->second;
    Channel *chan = new Channel(name);
    _channels[name] = chan;
    return chan;
}

Channel *ChannelManager::find(const std::string &name)
{
    std::map<std::string, Channel*>::iterator it;
    it = _channels.find(name);
    if (it == _channels.end())
        return NULL;
    return it->second;
}

void ChannelManager::removeIfEmpty(const std::string &name)
{
    std::map<std::string, Channel*>::iterator it;
    it = _channels.find(name);
    if (it == _channels.end())
        return ;

    if (it->second->isEmpty())
    {
        delete it->second;
        _channels.erase(it);
    }
}