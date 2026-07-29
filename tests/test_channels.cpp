// test_channels.cpp
#include "../Includes/Channel.hpp"
#include "../Includes/ChannelManager.hpp"
#include <iostream>
#include <cassert>

class Client {};

int main() {
    ChannelManager mgr;
    Client a, b, c;

    // --- getOrCreate / find ---
    assert(mgr.find("#foo") == NULL);
    Channel* chan = mgr.getOrCreateChan("#foo");
    assert(chan != NULL);
    assert(mgr.getOrCreateChan("#foo") == chan); // same object returned

    // --- membership ---
    assert(chan->isEmpty());
    chan->addClient(&a);
    chan->addClient(&b);
    assert(chan->getMemberCount() == 2);
    assert(chan->isMember(&a));
    assert(!chan->isMember(&c));

    // --- operators ---
    chan->addOperator(&a);
    assert(chan->isOperator(&a));
    assert(!chan->isOperator(&b));

    // operator status shouldn't apply on someone not in the channel
    chan->addOperator(&c);
    assert(!chan->isOperator(&c));

    // --- topic + restriction ---
    chan->setTopic("hello world");
    assert(chan->getTopic() == "hello world");
    assert(chan->hasTopic());

    assert(!chan->isTopicRestricted());
    chan->setTopicRestricted(true);
    assert(chan->isTopicRestricted());

    // --- kick: non-operator should fail ---
    chan->addClient(&c);
    assert(chan->getMemberCount() == 3);

    // simulate handleKick logic: b (not operator) tries to kick c
    bool bIsOperator = chan->isOperator(&b);
    assert(!bIsOperator); // so kick would be rejected
    assert(chan->getMemberCount() == 3); // nothing changed

    // a (operator) kicks c
    assert(chan->isOperator(&a));
    assert(chan->isMember(&c));
    chan->removeClient(&c);
    assert(chan->getMemberCount() == 2);
    assert(!chan->isMember(&c));

    // --- part ---
    chan->removeClient(&b);
    assert(chan->getMemberCount() == 1);
    assert(!chan->isEmpty());

    chan->removeClient(&a);
    assert(chan->isEmpty());

    // --- cleanup via manager ---
    mgr.removeIfEmpty("#foo");
    assert(mgr.find("#foo") == NULL);

    std::cout << "All tests passed!" << std::endl;
    return 0;
}