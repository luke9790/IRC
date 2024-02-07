#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
public:
    Client(int fd);
    void setNickname(const std::string& nick);
    const std::string& getNickname() const;

    int socket_fd;
    std::string nickname;
    bool isRegistered;
    bool hasReceivedNick;
    bool hasReceivedUser;

private:
    // Add more client-specific information
};

#endif // CLIENT_HPP
