#ifndef CLIENT_HPP
#define CLIENT_HPP

#include<vector>
#include<algorithm>
#include <string>

class Client {
public:
    Client(int fd);
    void setNickname(const std::string& nick);
    const std::string& getNickname() const;
    bool isInChannel(std::string channel_name);
    void removeChannel(std::string channel_name);
    void setIsJoin();
    int getIsJoin();
    void mergeBuffer(char *client_buffer);
    void clearBuffer();
    std::string getBuffer();
    std::string getUsername();
    
    int socket_fd;
    std::string nickname;
    std::string username;
    std::string realname;
    std::vector<std::string> channels;
    std::string buffer;
    bool auth;
    bool isRegistered;
    bool hasReceivedNick;
    bool hasReceivedUser;
    int isjoin;

private:
    // Add more client-specific information
};

#endif // CLIENT_HPP
