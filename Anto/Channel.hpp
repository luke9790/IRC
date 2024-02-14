// Channel.hpp
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "Client.hpp"
#include <sys/socket.h>

class Channel {
public:
    Channel(std::string channelName, int fd);
    ~Channel();

    void addClient(Client* client);
    void removeClient(Client* client);
    void setName(const std::string& name);
    const std::string& getName() const;
    void setTopic(const std::string& topic);
    const std::string& getTopic() const;
    std::vector<Client*> getClients() const;
    bool isClientInChannel(int client_fd) const;
    void setChannelOperator(int fd_op);
    bool isOperator(int client_fd);

    int getUserCount() const;
    void broadcast(const std::string& message);
private:
    std::vector<int> operators;
    std::vector<Client*> clients;
    std::string name;
    std::string topic;
    int userCount;
};

#endif // CHANNEL_HPP
