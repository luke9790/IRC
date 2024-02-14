#include "Handler.hpp"
#include "IRCServ.hpp"

// Funzione per stampare i contenuti di cmdParams
void stampaCmdParams(const std::vector<std::string>& cmdParams) {
    std::cout << "cmdParams contiene: ";
    for (size_t i = 0; i < cmdParams.size(); ++i) {
        std::cout << "'" << cmdParams[i] << "' ";
    }
    std::cout << std::endl;
}

int Handler::handleCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels, Client &client)
{
    if (cmdParams.empty()) return 0;
    // temporaneo

    // Stampa cmdParams per debugging
    stampaCmdParams(cmdParams);

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
    } else if (cmd == "LIST") {
        handleListCommand(client_fd, channels, clients);
    } else if (cmd == "QUIT") {
        handleQuitCommand(client_fd, clients, channels);
        return 1; // Segnala che il client vuole disconnettersi
    } else if (cmd == "PING") {
        handlePingCommand(client_fd, cmdParams);
    } else if (cmd == "WHO") {
        handleWhoCommand(client_fd, cmdParams);
    } else if (cmd == "USERHOST") {
        handleUserHostCommand(client_fd, cmdParams);
    } else {
        
        std::string target;
        std::string message = cmdParams[0];
        for (size_t i = 1; i < cmdParams.size(); ++i) {
            message += " " + cmdParams[i];
        }
        int flg = 0;
        
        std::string channel_name = client.getChannel();
        if (!channel_name.empty())
        {
            Channel *chan_ptr = channels[channel_name];
            std::vector<Client*> clients_ptr = chan_ptr->getClients();
            for (std::vector<Client*>::iterator it = clients_ptr.begin(); it != clients_ptr.end(); ++it)
            {
                if (*it != NULL && (*it)->socket_fd == client_fd)
                {
                    target = (*it)->getChannel();
                    flg = 1;
                    break;
                }
            }
            if (flg)
            {
                Channel* channel = channels[target];
                std::vector<Client*> clientsInChannel = channel->getClients();
                for (size_t i = 0; i < clientsInChannel.size(); ++i) {
                    send(clientsInChannel[i]->socket_fd, message.c_str(), message.length(), 0);
                }
            }
        }
    }
    return 0;
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

void Handler::handlePingCommand(int client_fd, const std::vector<std::string>& cmdParams) {
    if (cmdParams.size() >= 2) {
        std::string pongResponse = "PONG " + cmdParams[1] + "\r\n";
        send(client_fd, pongResponse.c_str(), pongResponse.length(), 0);
    }
}

void Handler::handleWhoCommand(int client_fd, const std::vector<std::string>& cmdParams) {
    // Risposta semplice per demo, adattala alle tue necessità
    std::string whoResponse = ":YourServer 352 " + cmdParams[1] + " ...\r\n";
    whoResponse += ":YourServer 315 " + cmdParams[1] + " :End of WHO list\r\n";
    send(client_fd, whoResponse.c_str(), whoResponse.length(), 0);
}

void Handler::handleUserHostCommand(int client_fd, const std::vector<std::string>& cmdParams) {
    // Risposta semplice per demo
    (void)cmdParams;
    std::string userhostResponse = ":YourServer 302 :userhost reply\r\n";
    send(client_fd, userhostResponse.c_str(), userhostResponse.length(), 0);
}

void Handler::handleListCommand(int client_fd, std::map<std::string, Channel*>& channels, std::map<int, Client*>& clients) {
    std::string serverName = "YourServer"; // Customize with your actual server name
    std::string client_name = clients[client_fd]->getNickname();

    
    // Inizia con l'invio del codice di inizio LIST
    std::string message = ":" + serverName + " 321 " + client_name + " Channel :Users  Name\r\n";
    send(client_fd, message.c_str(), message.length(), 0);

    // Itera sui canali e invia le informazioni di ciascuno
    for (std::map<std::string, Channel*>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
        Channel* channel = it->second;
        std::string channelName = channel->getName();
        int userCount = channel->getClients().size(); // Ottieni il numero di clienti nel canale
        std::string topic = channel->getTopic();

        std::stringstream ss;
        ss << userCount;
        std::string userCountStr = ss.str();
    
        // Formato: <channel> <# visible> :<topic>
        message = ":" + serverName + " 322 " + " " + client_name + " " + channelName + " " + userCountStr + " :" + topic + "\r\n";
        send(client_fd, message.c_str(), message.length(), 0);
    }

    // Termina con il codice di fine LIST
    message = ":" + serverName + " 323 " + client_name + " :End of /LIST\r\n";
    send(client_fd, message.c_str(), message.length(), 0);
}


void Handler::handleJoinCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels) {
    if (cmdParams.size() < 2) {
        // Comando JOIN malformato, potresti inviare un messaggio di errore.
        return;
    }
    std::string channelName = cmdParams[1];
    channelName.erase(0, 1);
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

void Handler::handleQuitCommand(int client_fd, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels) {
    std::map<std::string, Channel*>::iterator ch_it;
    for (ch_it = channels.begin(); ch_it != channels.end(); ++ch_it) {
        Channel* channel = ch_it->second;
        channel->removeClient(clients[client_fd]);
        // Qui potresti inviare una notifica ai membri del canale
    }

    // Chiudi il socket
    close(client_fd);
    delete clients[client_fd];
    clients.erase(client_fd);
    // Non possiamo rimuovere client_fd da master_set qui, dovrebbe essere gestito nel server principale
}



void Handler::handlePrivmsgCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels) {
    // temporaneo
    (void)client_fd;
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

