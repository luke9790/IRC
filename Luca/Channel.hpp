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
    void removeChannelOperator(int fd_op);

    void setName(const std::string& name);
    void setTopic(const std::string& topic);
    void setChannelOperator(int fd_op);
    void setMode_t(bool flg);
    void setUserLimits(int limits);
    void setPassword(std::string pass);
    void setInviteOnly(bool value);

    int getUserCount() const;
    const std::string& getName() const;
    std::vector<Client*> getClients() const;
    const std::string& getTopic() const;
    int getUserLimits();
    std::string getPassword();
    bool getInviteOnly();
    bool getMode_t();
    
    bool isOperator(int client_fd);
    bool isClientInChannel(int client_fd) const;

    void inviteClient(int client_fd); // Invita un client al canale
    bool isInvited(int client_fd) const; // Verifica se un client è stato invitato
    void removeInvitedClient(int client_fd);
    
    void broadcast(const std::string& message);
private:
    std::vector<int> invitedClients; // serve per controllare se abbiamo invitato clients
    std::vector<int> operators;
    std::vector<Client*> clients;
    std::string name;
    std::string topic;
    int userCount;
    bool topic_mode;
    int user_limits;
    bool invite_only;
    std::string pwd;
};

#endif // CHANNEL_HPP
