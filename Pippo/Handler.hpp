#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include "IRCServ.hpp"

class Client;
class Channel;
class IRCServ;

class Handler {
public:
    static int handleCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels, Client &client/* , IRCServ *serv */);
    
    static void handleQuitCommand(int client_fd, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels); // pubblica da chioccia
    
    
private:
    
    static void checkAndRegisterClient(int client_fd, std::map<int, Client*>& clients);
    static void sendWelcomeMessages(int client_fd, const std::string& nick);
    static void handleNickCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients);
    static void handleUserCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients);
    static void handleListCommand(int client_fd, std::map<std::string, Channel*>& channels, std::map<int, Client*>& clients);
    
    static void handlePingCommand(int client_fd, const std::vector<std::string>& cmdParams);
    static void handleWhoCommand(int client_fd, const std::vector<std::string>& cmdParams);
    static void handleUserHostCommand(int client_fd, const std::vector<std::string>& cmdParams);
    static void handleJoinCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels);
    static void handlePartCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels);
    static void handlePrivmsgCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels);
    static void handleUserKickCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels);
    static void handleUserInviteCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels);
    static void handleModeCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<std::string, Channel*>& channels);
    static void handleTopicCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels);
    // Add more command handlers as needed    
};


#endif // HANDLER_HPP
