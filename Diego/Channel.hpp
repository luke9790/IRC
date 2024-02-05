#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "Client.hpp"

class Channel {
public:
    void addClient(Client* client);
    void removeClient(Client* client);
    void setTopic(const std::string& topic);
    const std::string& getTopic() const;

private:
    std::vector<Client*> clients;
    std::string topic;
    // Additional channel properties
};

#endif // CHANNEL_HPP
