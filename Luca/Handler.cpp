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
        handleNickCommand(client_fd, cmdParams, clients);
    } else if (cmd == "USER") {
        handleUserCommand(client_fd, cmdParams, clients);
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

void Handler::handleNewConnection(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels) {
    // temporaneo
    (void)channels;

    // Send initial welcome messages using client->fd and client->nickname
    // Adjust this to fit your actual message sending logic
    handleNickCommand(client_fd, cmdParams, clients);
    clients[client_fd]->hasReceivedNick = true;
    handleUserCommand(client_fd, cmdParams, clients);
    clients[client_fd]->hasReceivedUser = true;
    clients[client_fd]->isRegistered = true;
    sendWelcomeMessages(client_fd, clients[client_fd]->nickname);
}

void Handler::sendWelcomeMessages(int client_fd, const std::string& nick) {
    std::string serverName = "YourServer"; // Customize with your actual server name
    std::string serverVersion = "YourServerVersion"; // Customize this
    std::string serverCreationDate = "YourServerCreationDate"; // Customize this
    std::string username = "YourUsername"; // This should be retrieved or set for the connected user
    std::string host = "YourHost"; // This should be the client's host or your server's host

    // Welcome message
    std::string message = ":" + serverName + " 001 " + nick + " :Welcome to the IRC network " + nick + "!" + username + "@" + host + "\r\n";
    send(client_fd, message.c_str(), message.length(), 0);

    // Your host message
    message = ":" + serverName + " 002 " + nick + " :Your host is " + serverName + ", running version " + serverVersion + "\r\n";
    send(client_fd, message.c_str(), message.length(), 0);

    // Server creation date message
    message = ":" + serverName + " 003 " + nick + " :This server was created " + serverCreationDate + "\r\n";
    send(client_fd, message.c_str(), message.length(), 0);

    // Server version message
    message = ":" + serverName + " 004 " + nick + " " + serverName + " " + serverVersion + " o o\r\n";
    send(client_fd, message.c_str(), message.length(), 0);

    // Example for sending a channel list (this is just an example, modify as needed)
    message = ":" + serverName + " 322 " + nick + " Channel 3 :Topic\r\n";
    send(client_fd, message.c_str(), message.length(), 0);

    // End of /LIST message
    message = ":" + serverName + " 323 " + nick + " :End of /LIST\r\n";
    send(client_fd, message.c_str(), message.length(), 0);
}

void Handler::handleNickCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients) {
    if (cmdParams.size() < 2) {
        // Errore: comando NICK senza parametri
        return;
    }
    std::string nick = cmdParams[1];
    clients[client_fd]->nickname = nick;
}

void Handler::handleUserCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients) {
    if (cmdParams.size() < 4) {
        // Errore: comando USER senza abbastanza parametri
        return;
    }
}


// void handleJoinCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<std::string, Channel*>& channels, std::map<int, Client*>& clients);
// void handlePrivmsgCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<std::string, Channel*>& channels, std::map<int, Client*>& clients);
