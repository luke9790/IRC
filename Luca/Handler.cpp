#include "Handler.hpp"
#include "IRCServ.hpp"

void Handler::handleCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels)
{
    if (cmdParams.empty()) return;
    // temporaneo
    (void)clients;
    (void)channels;

    const std::string& cmd = cmdParams[0];
    if (cmd == "CAP") {
        if (cmdParams.size() >= 2 && cmdParams[1] == "LS") {
            // Rispondi con un elenco vuoto di capacità o con CAP NAK per rifiutare la richiesta
            std::string capResponse = ":YourServer CAP * LS :\r\n"; // Elenco vuoto di capacità
            send(client_fd, capResponse.c_str(), capResponse.length(), 0);
        }
    } else if (cmd == "NICK") {
        handleNickCommand(client_fd, cmdParams, clients);
    } else if (cmd == "USER") {
        handleUserCommand(client_fd, cmdParams, clients);
    } else if (cmd == "JOIN") {
        handleJoinCommand(client_fd, cmdParams, clients, channels);
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
        // Potresti voler inviare un messaggio di errore al client
        return;
    }
    std::string nick = cmdParams[1];
    clients[client_fd]->nickname = nick;
    clients[client_fd]->hasReceivedNick = true;

    // Verifica se entrambi NICK e USER sono stati ricevuti
    checkAndRegisterClient(client_fd, clients);
}

void Handler::handleUserCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients) {
    if (cmdParams.size() < 5) {
        // Potresti voler inviare un messaggio di errore al client
        return;
    }
    std::string realname = cmdParams[4];
    for (size_t i = 5; i < cmdParams.size(); ++i) {
        realname += " " + cmdParams[i];
    }
    clients[client_fd]->realname = realname;
    clients[client_fd]->hasReceivedUser = true;

    // Verifica se entrambi NICK e USER sono stati ricevuti
    checkAndRegisterClient(client_fd, clients);
}

void Handler::checkAndRegisterClient(int client_fd, std::map<int, Client*>& clients) {
    Client* client = clients[client_fd];
    if (client && client->hasReceivedNick && client->hasReceivedUser && !client->isRegistered) {
        client->isRegistered = true;
        sendWelcomeMessages(client_fd, client->nickname);
    }
}

void Handler::handleJoinCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels) {
    if (cmdParams.size() < 2) {
        // Comando JOIN malformato, potresti inviare un messaggio di errore.
        return;
    }
    std::string channelName = cmdParams[1];
    if (channels.find(channelName) == channels.end()) {
        channels[channelName] = new Channel(channelName); // Crea il canale se non esiste
    }
    Channel* channel = channels[channelName];
    channel->addClient(clients[client_fd]); // Aggiungi il client al canale

    // DA FARE: inviare una notifica nel canale dell'arrivo del nuovo utente
}

void Handler::handlePartCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels) {
    if (cmdParams.size() < 2) {
        // Potresti voler inviare un messaggio di errore al client per comando PART malformato
        return;
    }
    std::string channelName = cmdParams[1];
    if (channels.find(channelName) != channels.end()) {
        Channel* channel = channels[channelName];
        channel->removeClient(clients[client_fd]);
        // Opzionale: inviare una notifica ai restanti membri del canale
    } else {
        // Opzionale: inviare un messaggio di errore al client per canale non trovato
    }
}

void Handler::handlePrivmsgCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels) {
    if (cmdParams.size() < 3) {
        // Potresti voler inviare un messaggio di errore al client per comando PRIVMSG malformato
        return;
    }
    std::string target = cmdParams[1];
    std::string message = cmdParams[2];
    for (size_t i = 3; i < cmdParams.size(); ++i) {
        message += " " + cmdParams[i];
    }

    if (target[0] == '#') { // Il messaggio è destinato a un canale
        if (channels.find(target) != channels.end()) {
            Channel* channel = channels[target];
            std::vector<Client*> clientsInChannel = channel->getClients();
            for (size_t i = 0; i < clientsInChannel.size(); ++i) {
                send(clientsInChannel[i]->socket_fd, message.c_str(), message.length(), 0);
            }
        } else {
            // Opzionale: inviare un messaggio di errore al client per canale non trovato
        }
    } else { // Il messaggio è destinato a un utente singolo
        for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second->getNickname() == target) {
                send(it->second->socket_fd, message.c_str(), message.length(), 0);
                break; // Messaggio inviato, esce dal ciclo
            }
        }
        // Opzionale: gestire il caso in cui il nickname non è trovato
    }
}

