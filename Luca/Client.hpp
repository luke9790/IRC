#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
public:
    Client(int fd);
    void setNickname(const std::string& nick);
    const std::string& getNickname() const;
    void setChannel(std::string chan);
    const std::string& getChannel() const;
    void setIsJoin();
    int getIsJoin();
    void mergeBuffer(char *client_buffer);
    void clearBuffer();
    std::string getBuffer();
    
    int socket_fd;
    std::string nickname;
    std::string username;
    std::string realname;
    std::string channel;
    std::string buffer;
    bool isRegistered;
    bool hasReceivedNick;
    bool hasReceivedUser;
    int isjoin;

private:
    // Add more client-specific information
};

#endif // CLIENT_HPP
