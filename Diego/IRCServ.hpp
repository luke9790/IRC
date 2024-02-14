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
#include <cerrno>

class Handler;

class IRCServ {
public:
    IRCServ(int port, const std::string& password);
    ~IRCServ();

    void run();

private:
    int port;
    std::string password;
    int server_fd;
    std::map<int, Client*> clients; // Maps socket fd to Client objects
    std::map<std::string, Channel*> channels; // Maps channel names to Channel objects
};

#endif
