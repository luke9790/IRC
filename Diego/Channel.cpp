#include "Channel.hpp"

Channel::Channel(std::string channelName, int fd) : name(channelName), topic(""), userCount(0), topic_mode(false) {
    setChannelOperator(fd);
}

void Channel::setChannelOperator(int fd_op)
{
    operators.push_back(fd_op);
}

void Channel::removeChannelOperator(int fd_op)
{
    std::vector<int>::iterator it;
    for(it = operators.begin(); it != operators.end(); it++)
    {
        if (*it == fd_op)
        {
            it = operators.erase(it);
            break;
        }
    }
}

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
    userCount++;
    clients.push_back(client); // Aggiungi il client al canale
}

void Channel::removeClient(Client* client) {
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (*it == client) {
            clients.erase(it); // Rimuovi il client dal canale
            userCount--;
            break;
        }
    }
}


bool Channel::isOperator(int client_fd)
{
    for (std::vector<int>::iterator it = operators.begin(); it != operators.end(); ++it) {
        if (client_fd == *it)
            return true;
    }
    return false;
}

bool Channel::isClientInChannel(int client_fd) const {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i]->socket_fd == client_fd) {
            return true; // Il client è presente nel canale
        }
    }
    return false; // Il client non è stato trovato nel canale
}

void Channel::broadcast(const std::string& message) {
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        Client* client = *it;
        // Controlla se il client corrente deve essere escluso dalla broadcast
        if (client->socket_fd != -1) {
            send(client->socket_fd, message.c_str(), message.length(), 0);
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

int Channel::getUserCount() const
{
    return userCount;
}

void Channel::setMode_t(bool flg)
{
    topic_mode = flg;
}

void Channel::setInviteOnly(bool value)
{
    invite_only = value;
}

void Channel::setUserLimits(int limits)
{
    user_limits = limits;
}

void Channel::setPassword(std::string pass)
{
    pwd = pass;
}

int Channel::getUserLimits()
{
    return user_limits;
}

std::string Channel::getPassword()
{
    return pwd;
}

bool Channel::getInviteOnly()
{
    return invite_only;
}