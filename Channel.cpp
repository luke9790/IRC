#include "Channel.hpp"

void Channel::addClient(Client* client) {
    // Logic to add a client to the channel
}

void Channel::removeClient(Client* client) {
    // Logic to remove a client from the channel
}

void Channel::setTopic(const std::string& topic) {
    this->topic = topic;
}

const std::string& Channel::getTopic() const {
    return topic;
}
