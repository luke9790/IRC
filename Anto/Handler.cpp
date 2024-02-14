#include "Handler.hpp"
#include "IRCServ.hpp"

void sendChannelUserList(int client_fd, Channel *channel) {
    std::string userList;
    std::vector<Client*> clients = channel->getClients();
    for(size_t i = 0; i < clients.size(); i++)
    {
        if (i != 0)
            userList = userList + " @" + clients[i]->getNickname();
        else
            userList = "@" + clients[i]->getNickname();
    }
    std::string response = ":YourServer 353 nickname @ " + channel->getName() + " :" + userList + "\r\n";
    send(client_fd, response.c_str(), response.length(), 0);
}

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
    (void)client;
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
    } else if (cmd == "PART") {
        handlePartCommand(client_fd, cmdParams, clients, channels);
    } else if (cmd == "PRIVMSG") {
        handlePrivmsgCommand(client_fd, cmdParams, clients, channels);
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
    } else if (cmd == "KICK") {
        handleUserKickCommand(client_fd, cmdParams, clients, channels);
    } else if (cmd == "MODE") {
        handleModeCommand(client_fd, cmdParams, channels);
    } else if (cmd == "INVITE") {
        handleUserInviteCommand(client_fd, cmdParams, clients, channels);
    } else if (cmd == "TOPIC") {
        handleTopicCommand(client_fd, cmdParams, clients, channels);
    } else {
        // Comando non riconosciuto
        std::string errorMsg = ":YourServer 421 " + cmdParams[0] + " :Unknown command\r\n";
        send(client_fd, errorMsg.c_str(), errorMsg.length(), 0);
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
        // Invia un messaggio di errore al client indicando un comando NICK malformato
        std::string errorMsg = ":YourServer 431 :No nickname given\r\n";
        send(client_fd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    std::string newNick = cmdParams[1];
    std::string oldNick = clients[client_fd]->getNickname();
    std::string user = clients[client_fd]->username;
    std::string host = "YourHost"; // Sostituisci con l'host effettivo del client se disponibile
    if(newNick != oldNick){
        clients[client_fd]->setNickname(newNick);
        clients[client_fd]->hasReceivedNick = true;
    }

    // Verifica se entrambi NICK e USER sono stati ricevuti
    checkAndRegisterClient(client_fd, clients);

    // Notifica il cambio di nickname
    std::string nickChangeMsg = ":" + oldNick + "!" + user + "@" + host + " NICK :" + newNick + "\r\n";
    send(client_fd, nickChangeMsg.c_str(), nickChangeMsg.length(), 0);

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
    clients[client_fd]->username = cmdParams[1];
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
        int userCount = channel->getUserCount();
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
    if (channelName[0] != '#') {
        channelName = "#" + channelName; // Assicura che il nome del canale inizi con "#"
    }
    if (channels.find(channelName) == channels.end()) {
        channels[channelName] = new Channel(channelName, client_fd); // Crea il canale se non esiste
    }
    Channel* channel = channels[channelName];
    Client* client = clients[client_fd];
    channel->addClient(client);
    std::string joinMessage = ":" + client->nickname + "!" + client->username + "@YourServer JOIN " + channelName + "\r\n";
    channel->broadcast(joinMessage);
    std::vector<Client*> clients_list = channel->getClients(); 
    for (size_t i = 0; i < clients_list.size(); i++)
    {
        sendChannelUserList(clients_list[i]->socket_fd, channel);
    } 
    // Formattazione corretta del messaggio di JOIN
}

void Handler::handlePartCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels) {
    if (cmdParams.size() < 2) {
        // Potresti voler inviare un messaggio di errore al client per comando PART malformato
        return;
    }
    std::string channelName = cmdParams[1];
    // Assicurati che il nome del canale includa il prefisso '#'
    if (channelName[0] != '#') {
        channelName = "#" + channelName;
    }

    if (channels.find(channelName) != channels.end()) {
        Channel* channel = channels[channelName];
        if (channel->isClientInChannel(client_fd)) {
            Client* client = clients[client_fd];
            channel->removeClient(client);
            std::vector<Client*> clients_list = channel->getClients(); 
            for (size_t i = 0; i < clients_list.size(); i++)
            {
                sendChannelUserList(clients_list[i]->socket_fd, channel);
            }
            // NOTIFICHIAMO a tutti che qualcuno e' uscito dal canale
            char hostname[1024]; // Buffer per ospitare il nome dell'host
            hostname[1023] = '\0'; // Assicurati che ci sia il terminatore alla fine
            gethostname(hostname, 1023);
            std::string partMessage = ":" + client->nickname + "!" + client->username + "@"+ hostname + " PART " + channelName + " :Reason" + "\r\n";
            channel->broadcast(partMessage); // Supponendo che tu abbia un metodo broadcast per inviare messaggi a tutti i client nel canale
            send(client_fd, partMessage.c_str(), partMessage.length(), 0);
            std::cout << partMessage << std::endl;
        } else {
            // Il client non è nel canale, gestisci l'errore
        }
    } else {
        // Canale non trovato, gestisci l'errore
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
    if (cmdParams.size() < 3) {
        // Potresti voler inviare un messaggio di errore al client per comando PRIVMSG malformato
        return;
    }
    std::string target = cmdParams[1];
    std::string message = cmdParams[2];
    for (size_t i = 3; i < cmdParams.size(); ++i) {
        message += " " + cmdParams[i];
    }

    // Ottieni il nickname del mittente dal client_fd
    std::string mittenteNickname;
    if (clients.find(client_fd) != clients.end()) {
        mittenteNickname = clients[client_fd]->getNickname();
    } else {
        // Gestire il caso in cui il client_fd non è trovato nei client connessi
        return; // o invia un messaggio di errore appropriato
    }

    if (target[0] == '#') { // Il messaggio è destinato a un canale
        if (channels.find(target) != channels.end()) {
            Channel* channel = channels[target];
            if (!channel->isClientInChannel(client_fd)) {
                // Se il mittente non è nel canale, invia un messaggio di errore al mittente
                std::string errorMsg = ":YourServer 404 " + target + " :Cannot send to channel\r\n";
                send(client_fd, errorMsg.c_str(), errorMsg.length(), 0);
                return;
            }
            // Inoltra il messaggio a tutti i membri del canale
            std::vector<Client*> clientsInChannel = channel->getClients();
            for (size_t i = 0; i < clientsInChannel.size(); ++i) {
                if (clientsInChannel[i]->socket_fd != client_fd)
                {
                    std::string fullMessage = ":" + mittenteNickname + " PRIVMSG " + target + " " + message + "\r\n";
                    send(clientsInChannel[i]->socket_fd, fullMessage.c_str(), fullMessage.length(), 0);
                }
            }
        } else {
            // Opzionale: inviare un messaggio di errore al client per canale non trovato
        }
    } else { // Il messaggio è destinato a un utente singolo
        bool destinatarioTrovato = false;
        for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second->getNickname() == target) {
                std::string fullMessage = ":" + mittenteNickname + " PRIVMSG " + target + " " + message + "\r\n";
                send(it->second->socket_fd, fullMessage.c_str(), fullMessage.length(), 0);
                destinatarioTrovato = true;
                break; // Messaggio inviato, esce dal ciclo
            }
        }
        if (!destinatarioTrovato) {
            // Opzionale: invia un messaggio di errore al mittente se il destinatario non è trovato
        }
    }
}


void Handler::handleUserKickCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels) {
    
    bool isInside = false;
    Channel *act_chnl;
    std::string channel_name = cmdParams[1];
    (void)client_fd;
    (void)cmdParams;
    (void)clients;
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); it++)
    {
        std::cout << it->first << std::endl;
        if (it->second->getName() == channel_name)
        {
            act_chnl = it->second;
        }
    }
    std::vector<Client*> chnl_clients = act_chnl->getClients();
    (void)chnl_clients;
    size_t i;
    for(i = 0; i < chnl_clients.size(); i++)
    {
        if (chnl_clients[i]->getNickname() == cmdParams[2])
        {
            isInside = true;
            break;
        }
    }
    if (act_chnl->isOperator(client_fd) && isInside)
    {
        std::string kickMessage;
        std::string finalMessage;
        kickMessage = cmdParams[3];
        kickMessage.erase(0, 1);
        if (kickMessage != "User")
        {
            finalMessage = "KICK " + act_chnl->getName() + " " + chnl_clients[i]->getNickname() + " :" + clients[client_fd]->getNickname() + " " + kickMessage + "\r\n";
        }
        else
        {
            finalMessage = "KICK " + act_chnl->getName() + " " + chnl_clients[i]->getNickname() + " :" + clients[client_fd]->getNickname() + " diocane del dio" + "\r\n";
        }
    // Invia il messaggio al client
        std::string broad_msg;
        act_chnl->removeClient(chnl_clients[i]);
        send(chnl_clients[i]->socket_fd, finalMessage.c_str(), finalMessage.length(), 0);
        std::string fullMessage;
        for(size_t j = 0; j < chnl_clients.size(); j++)
        {
            if (chnl_clients[i]->socket_fd != chnl_clients[j]->socket_fd)
            {
                fullMessage = ":server PRIVMSG " + channel_name + " :" + chnl_clients[i]->getNickname() + " è stato tirato fuori dalle palle\r\n";
            }
            else
                fullMessage = ":server PRIVMSG " + channel_name + " :sei stato tirato fuori dalle palle\r\n";

            send(chnl_clients[j]->socket_fd, fullMessage.c_str(), fullMessage.length(), 0);
        }
        std::vector<Client*> clients_list = act_chnl->getClients(); 
        for (size_t i = 0; i < clients_list.size(); i++)
        {
            sendChannelUserList(clients_list[i]->socket_fd, act_chnl);
        }
    }
    else
    {
        std::string finalMessage;
        if(act_chnl->isOperator(client_fd))
        {    
            finalMessage = ":server PRIVMSG " + channel_name + " :non c'è il tipo\r\n";
        }
        else
            finalMessage = ":server PRIVMSG " + channel_name + " :potresti, ma non sei il meglio\r\n";
        send(client_fd, finalMessage.c_str(), finalMessage.length(), 0);
    }
    
}


void Handler::handleUserInviteCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels) {
    (void)client_fd;
    (void)cmdParams;
    (void)clients;
    (void)channels;
}


void Handler::handleModeCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<std::string, Channel*>& channels) {
    (void)client_fd;
    (void)cmdParams;
    (void)channels;
}


void Handler::handleTopicCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels) {
    (void)client_fd;
    (void)cmdParams;
    (void)channels;
    (void)clients;
}
