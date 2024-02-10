// Channel.hpp
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "Client.hpp"

class Channel {
public:
    Channel(std::string channelName);
    ~Channel();

    void addClient(Client* client);
    void removeClient(Client* client);
    void setName(const std::string& name);
    const std::string& getName() const;
    void setTopic(const std::string& topic);
    const std::string& getTopic() const;
    std::vector<Client*> getClients() const;
private:
    std::vector<Client*> clients;
    std::string name;
    std::string topic;
};

#endif // CHANNEL_HPP
