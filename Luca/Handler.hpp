#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <vector>
#include <map>
#include <string>

class Client;
class Channel;

class Handler {
public:
    static void handleNewConnection(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels);
    static void handleCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels);

private:
    static void sendWelcomeMessages(int client_fd, const std::string& nick);
    static void handleNickCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients);
    static void handleUserCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients);
    static void handleJoinCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<std::string, Channel*>& channels, std::map<int, Client*>& clients);
    static void handlePrivmsgCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<std::string, Channel*>& channels, std::map<int, Client*>& clients);
    // Add more command handlers as needed
};


#endif // HANDLER_HPP
