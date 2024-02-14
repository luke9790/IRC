#include "Channel.hpp"

Channel::Channel(std::string channelName) : name(channelName), topic("") {}

Channel::~Channel() {
    // Pulizia delle risorse, se necessario
}

void Channel::addClient(Client* client) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i] == client) {
            return; // Il client è già nel canale
        }
    }
    client->setChannel(this->name);
    clients.push_back(client); // Aggiungi il client al canale
}

void Channel::removeClient(Client* client) {
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (*it == client) {
            clients.erase(it); // Rimuovi il client dal canale
            break;
        }
    }
}

void Channel::setName(const std::string& name) {
    this->name = name;
}

const std::string& Channel::getName() const {
    return name;
}

void Channel::setTopic(const std::string& topic) {
    this->topic = topic;
}

const std::string& Channel::getTopic() const {
    return topic;
}

std::vector<Client*> Channel::getClients() const{
    return clients;
}
