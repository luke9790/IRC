#ifndef IRC_SERV_HPP
#define IRC_SERV_HPP
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <cstring>
#include <vector>
#include <algorithm>
#include <sys/select.h>
#include <string>
#include "Client.hpp"
#include "Channel.hpp"
#include "CommandParser.hpp"
#include <iostream>
#include <map>


class IRCServ {
public:
    IRCServ(int port, const std::string& password);
    ~IRCServ();

    void run();

    void handleNickCommand(int clientFd, const std::vector<std::string>& params);
    void handleJoinCommand(int clientFd, const std::vector<std::string>& params);
    void handlePrivmsgCommand(int clientFd, const std::vector<std::string>& params);
    void handlePartCommand(int clientFd, const std::vector<std::string>& params);
    void handleKickCommand(int clientFd, const std::string& channel, const std::string& user);
    void handleInviteCommand(int clientFd, const std::string& user, const std::string& channel);
    void handleTopicCommand(int clientFd, const std::string& channel, const std::string& topic);
    void handleModeCommand(int clientFd, const std::vector<std::string>& params);

private:
    int port;
    std::string password;
    int server_fd;
    std::map<std::string, Client*> clients; // Maps socket fd to Client objects
    std::map<std::string, Channel*> channels; // Maps channel names to Channel objects
};

#endif
