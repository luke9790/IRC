#include "Handler.hpp"
#include "IRCServ.hpp"

void Handler::handleCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels)
{
    if (cmdParams.empty()) return;

    // temporaneo
    (void)clients;
    (void)channels;

    const std::string& cmd = cmdParams[0];
    if (cmd == "NICK") {
        // handleNickCommand(client_fd, cmdParams);
    } else if (cmd == "JOIN") {
        // handleJoinCommand(client_fd, cmdParams);
    } else if (cmd == "PRIVMSG") {
        // handlePrivmsgCommand(client_fd, cmdParams);
    } else {
        // Handling unrecognized commands by echoing the message back to the client
        std::string message = "Unrecognized command: ";
        for (std::vector<std::string>::const_iterator it = cmdParams.begin(); it != cmdParams.end(); ++it) {
            message += *it + " ";
        }
        // Ensure the message ends with a newline character
        message += "\n";

         // Log the unrecognized command
        std::cout << message << std::endl;

        // Send the echo back to the client
        send(client_fd, message.c_str(), message.length(), 0);
    }
}

// void handleNickCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients);
// void handleJoinCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<std::string, Channel*>& channels, std::map<int, Client*>& clients);
// void handlePrivmsgCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<std::string, Channel*>& channels, std::map<int, Client*>& clients);
