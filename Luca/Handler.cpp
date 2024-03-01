#include "Handler.hpp"
#include "IRCServ.hpp"

bool isAllNum(std::string str)
{
    for(size_t i = 0; i < str.size(); i++)
    {
        if(!isdigit(str[i]))
            return false;
    }
    return true;
}

void sendChannelUserList(int client_fd, Channel *channel) {
    std::string userList;
    std::vector<Client*> clients = channel->getClients();
    for(size_t i = 0; i < clients.size(); i++)
    {
        if (channel->isOperator(clients[i]->socket_fd))
	    {
            if (i != 0)
                userList = userList + " @" + clients[i]->getNickname();
            else
                userList = "@" + clients[i]->getNickname();
        } else {
            if (i != 0)
                userList = userList + " " + clients[i]->getNickname();
            else
                userList = clients[i]->getNickname();
	    }
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
    //stampaCmdParams(cmdParams);

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
        // if(!(cmd == "PASS"))
        // {
        //     std::string errorMsg = ":YourServer 421 " + cmdParams[0] + " :Unknown command\r\n";
        //     send(client_fd, errorMsg.c_str(), errorMsg.length(), 0);
        // }
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
    for (std::map<std::string, Channel*>::const_iterator it = channels.begin(); it != channels.end(); it++) {
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
    ///controllare se nel canale c'e' la pw e se e' invite only// darti il bona se il canale ha un user limit ed e' pieno o se la pw e' a cazzo e se non sei stato invitato
    if (cmdParams.size() < 2) {
        std::string errorMsg = ":YourServer 461 " + cmdParams[0] + " :Not enough parameters\r\n";
        send(client_fd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    std::string channelName = cmdParams[1];
    if (channelName[0] != '#') {
        channelName = "#" + channelName;
    }
    if (channels.find(channelName) == channels.end()) {
        channels[channelName] = new Channel(channelName, client_fd);

        if (cmdParams.size() > 2) { // se c'e' la password la settiamo per il nuovo canale
        Channel* newChannel = channels[channelName];
        newChannel->setPassword(cmdParams[2]);
    }
    }
    Channel* channel = channels[channelName];
    Client* client = clients[client_fd];

    // Controllo della password (se necessaria)
    bool passwordRequired = !channel->getPassword().empty();
    bool passwordCorrect = cmdParams.size() > 2 && channel->getPassword() == cmdParams[2];
    bool isInvited = channel->isInvited(client_fd);
    if (passwordRequired && !passwordCorrect && !isInvited) {
        // Invia un messaggio di errore per password errata
        std::string errorMsg = ":YourServer 475 " + channelName + " :Cannot join channel (+k) - bad key\r\n";
        send(client_fd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    
    // Controllo invite-only
    if (channel->getInviteOnly()) {
    if (!channel->isInvited(client_fd)) {
        // Invia un messaggio di errore per indicare che l'utente non è stato invitato
        std::string errorMsg = ":YourServer 473 " + channelName + " :Cannot join channel (+i) - you must be invited.\r\n";
        send(client_fd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
}
    
    // Controllo limite utenti
    if (channel->getUserLimits() > 0 && channel->getUserCount() >= channel->getUserLimits()) {
        // Invia un messaggio di errore per canale pieno
        std::string errorMsg = ":YourServer 471 " + channelName + " :Cannot join channel (+l) - channel is full\r\n";
        send(client_fd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    channel->addClient(client);
    channel->removeInvitedClient(client_fd);
    std::string joinMessage = ":" + client->nickname + "!" + client->username + "@YourServer JOIN " + channelName + "\r\n";
    channel->broadcast(joinMessage);
    std::vector<Client*> clients_list = channel->getClients(); 
    for (size_t i = 0; i < clients_list.size(); i++)
    {
        sendChannelUserList(clients_list[i]->socket_fd, channel);
    } 
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
    for (ch_it = channels.begin(); ch_it != channels.end(); ch_it++) {
        Channel* channel = ch_it->second;
        channel->removeClient(clients.find(client_fd)->second);
        // Qui potresti inviare una notifica ai membri del canale
    }

    // Chiudi il socket
    close(client_fd);

    // Elimina il cliente e rimuovilo dalla mappa dei clienti
    std::map<int, Client*>::iterator client_it = clients.find(client_fd);
    if (client_it != clients.end()) {
        delete client_it->second;
        clients.erase(client_it);
    }
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
    Channel *act_chnl = NULL;
    std::string channel_name = cmdParams[1];
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); it++)
    {
        std::cout << it->first << std::endl;
        if (it->second->getName() == channel_name)
        {
            act_chnl = it->second;
            break;
        }
    }
    std::vector<Client*> chnl_clients = act_chnl->getClients();
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
        for (i = 0; i < clients_list.size(); i++)
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
    if (cmdParams.size() != 3) {
        // Errore: Parametri sbagliati
        std::string errorMsg = ":YourServer 461 " + cmdParams[0] + " :Wrong number of parameters. \r\n";
        send(client_fd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    std::string targetNickname = cmdParams[1];
    std::string channelName = cmdParams[2];

     if (channelName[0] != '#') {
        channelName = "#" + channelName;
    }

    // Verifica se il canale esiste
    if (channels.find(channelName) == channels.end()) {
        std::string errorMsg = ":YourServer 403 " + channelName + " :No such channel\r\n";
        send(client_fd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    Channel* channel = channels[channelName];
    Client* sender = clients[client_fd];

    // Verifica se il mittente è nel canale
    if (!channel->isClientInChannel(client_fd)) {
        std::string errorMsg = ":YourServer 442 " + channelName + " :You're not on that channel\r\n";
        send(client_fd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    // Trova il destinatario nell'elenco dei client
    bool found = false;
    for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second->getNickname() == targetNickname) {
            found = true;
            // Invia l'invito al destinatario
            // Aggiunge l'invito al canale
            channel->inviteClient(it->first); // Utilizza l'identificativo del client

            std::string inviteMsg = ":" + sender->getNickname() + "!YourHost INVITE " + targetNickname + " :" + channelName + "\r\n";
            send(it->first, inviteMsg.c_str(), inviteMsg.length(), 0);

            // Feedback al mittente
            std::string feedbackMsg = ":YourServer 341 " + targetNickname + " " + channelName + "\r\n";
            send(client_fd, feedbackMsg.c_str(), feedbackMsg.length(), 0);
            break;
        }
    }


    if (!found) {
        // Utente non trovato
        std::string errorMsg = ":YourServer 401 " + targetNickname + " :No such nick/channel\r\n";
        send(client_fd, errorMsg.c_str(), errorMsg.length(), 0);
    }
}


void Handler::handleModeCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<std::string, Channel*>& channels) {

    std::string channel_name;
    std::string cmd;
    std::string value;
    std::string errorMesg;
    if(cmdParams.size() > 2)
    {
        channel_name = cmdParams[1];
        if (channels[channel_name])
        {
            std::string pm = "+-";
            std::string cmd_mode = "itkolb";
            cmd = cmdParams[2];
            if (cmd.size() == 2 && pm.find(cmd[0]) != std::string::npos && cmd_mode.find(cmd[1]) != std::string::npos)
            {
                if(cmd[1] == 'i')
                {
                	if (cmdParams.size() == 3)
                	{
                        	if (cmd[0] == '+')
                            		channels[channel_name]->setInviteOnly(true);
                       		else if (cmd[0] == '-')
                            		channels[channel_name]->setInviteOnly(false);
                            else
                            {
                                errorMesg = ":YourServer 421 " + channel_name + " :Unknown mode command\r\n";
                                send(client_fd, errorMesg.c_str(), errorMesg.length(), 0);
                            }
                	}
                	else
                	{
                        errorMesg = ":YourServer 421 " + channel_name + " :Unknown mode command\r\n";
                        send(client_fd, errorMesg.c_str(), errorMesg.length(), 0);
                	}
                }
                else if(cmd[1] == 'k')
                {
                    if (cmd[0] == '+')
                        {
                        	if (cmdParams.size() == 4 && cmdParams[3].size() != 0)
                        	{
                               		channels[channel_name]->setPassword(cmdParams[3]);
                        	}
                        	else
                        	{
                                errorMesg = ":YourServer 421 " + channel_name + " :Unknown mode command\r\n";
                                send(client_fd, errorMesg.c_str(), errorMesg.length(), 0);
                        	}
                        }
                	else if (cmd[0] == '-')
                	{
                       		if (cmdParams.size() == 3)
                        	{
                        		channels[channel_name]->setPassword("");
                        	}
                        	else
                 		    {
                                errorMesg = ":YourServer 421 " + channel_name + " :Unknown mode command\r\n";
                                send(client_fd, errorMesg.c_str(), errorMesg.length(), 0);
                        	}
                	}
                    else
                    {
                        errorMesg = ":YourServer 421 " + channel_name + " :Unknown mode command\r\n";
                        send(client_fd, errorMesg.c_str(), errorMesg.length(), 0);
                    }
                }
                else if(cmd[1] == 't')
                {
                     if (cmdParams.size() == 3)
                    {
                        if (cmd[0] == '+')
                        	channels[channel_name]->setMode_t(true);
                        else if (cmd[0] == '-')
                        	channels[channel_name]->setMode_t(false);
                        else
                        {
                            errorMesg = ":YourServer 421 " + channel_name + " :Unknown mode command\r\n";
                            send(client_fd, errorMesg.c_str(), errorMesg.length(), 0);
                        }
                    }
                    else
                    {
                        errorMesg = ":YourServer 421 " + channel_name + " :Unknown mode command\r\n";
                        send(client_fd, errorMesg.c_str(), errorMesg.length(), 0);
                    }
                }
                else if(cmd[1] == 'o')
                {
                    if (cmdParams.size() == 4)
                    {
                        std::vector<Client*> chnlclients = channels[channel_name]->getClients();
                        std::vector<Client*>::iterator it;
                        for(it = chnlclients.begin(); it != chnlclients.end(); it++)
                        {
                            if((*it)->getNickname() == cmdParams[3])
                                break;
                        }
                        if (it != chnlclients.end())
                        {
                        	if(cmd[0] == '+')
                                	channels[channel_name]->setChannelOperator((*it)->socket_fd);
                        	else if (cmd[0] == '-')
                                            channels[channel_name]->removeChannelOperator((*it)->socket_fd);
                            else
                            {
                                errorMesg = ":YourServer 421 " + channel_name + " :Unknown mode command\r\n";
                                send(client_fd, errorMesg.c_str(), errorMesg.length(), 0);
                            }
                        }
                        else
                        {
                            errorMesg = ":YourServer 421 " + channel_name + " :Unknown mode command\r\n";
                            send(client_fd, errorMesg.c_str(), errorMesg.length(), 0);
                        }
                    }
                }
                else if(cmd[1] == 'l')
                {
                        if (cmd[0] == '+')
                        {
                            if (cmdParams.size() == 4 && isAllNum(cmdParams[3]))
                            {
                                channels[channel_name]->setUserLimits(atoi(cmdParams[3].c_str()));
                            }
                            else
                            {
                                errorMesg = ":YourServer 421 " + channel_name + " :Unknown mode command\r\n";
                                send(client_fd, errorMesg.c_str(), errorMesg.length(), 0);
                            }
                        }
                        else if (cmd[0] == '-')
                        {
                            if (cmdParams.size() == 3)
                            {
                                channels[channel_name]->setUserLimits(0);
                            }
                            else
                            {
                                errorMesg = ":YourServer 421 " + channel_name + " :Unknown mode command\r\n";
                                send(client_fd, errorMesg.c_str(), errorMesg.length(), 0);
                            }
                        }
                        else
                        {
                            errorMesg = ":YourServer 421 " + channel_name + " :Unknown mode command\r\n";
                            send(client_fd, errorMesg.c_str(), errorMesg.length(), 0);
                        }
                }
                else if(cmd[1] == 'b'){
                        std::string endOfBanListMsg = ":YourServer 368 " + cmdParams[0] + " " + channel_name + " :End of channel ban list\r\n";
                        send(client_fd, endOfBanListMsg.c_str(), endOfBanListMsg.length(), 0);
                }
            }
            else
            {
                errorMesg = ":YourServer 421 " + channel_name + " :Unknown mode command\r\n";
                send(client_fd, errorMesg.c_str(), errorMesg.length(), 0);
            }
        }
        else
        {
            errorMesg = ":YourServer 421 " + channel_name + " :Unknown\r\n";
            send(client_fd, errorMesg.c_str(), errorMesg.length(), 0);
        }
    }
    else
    {
        channel_name = cmdParams[1];
        if (channels[channel_name])
        {
            // Il canale esiste, ottieni le sue modalità correnti
            std::cout << "Eccoci" <<std::endl;
            std::string currentModes = channels[channel_name]->getCurrentModes(); // restituisce una stringa con le modalità attive
            std::string modeResponse = ":YourServer 324 " + cmdParams[0] + " " + channel_name + " " + currentModes + "\r\n";
            std::cout << modeResponse << std::endl;
            send(client_fd, modeResponse.c_str(), modeResponse.length(), 0);
        }
        else
        {
            errorMesg = ":YourServer 421 " + channel_name + " :Unknown channel\r\n";
            send(client_fd, errorMesg.c_str(), errorMesg.length(), 0);
        }
        
    }
}


void Handler::handleTopicCommand(int client_fd, const std::vector<std::string>& cmdParams, std::map<int, Client*>& clients, std::map<std::string, Channel*>& channels) {
    /*if (cmdParams.size() < 2) {
        // Il comando è malformato, invia un messaggio di errore al client
        return;
    }*/
    std::string channelName = cmdParams[1];
    // Assicurati che il nome del canale includa il prefisso '#'
    if (channelName[0] != '#') {
        channelName = "#" + channelName;
    }

    if (channels.find(channelName) != channels.end()) {
        Channel* channel = channels[channelName];
        Client* client = clients[client_fd];

        // Verifica se il client è un operatore del canale
        if (channel->isOperator(client_fd)) {
            // Il client è un operatore, controlla se è specificato un nuovo topic
            if (cmdParams.size() >= 3) {
                std::string newTopic = cmdParams[2];
                for (size_t i = 3; i < cmdParams.size(); ++i) {
                    newTopic += " " + cmdParams[i];
                }
                channel->setTopic(newTopic);

                // Invia conferma al client
                std::string response = ":YourServer 332 " + client->getNickname() + " " + channelName + " " + newTopic + "\r\n";
                send(client_fd, response.c_str(), response.length(), 0);
            } else {
                // Il comando non specifica un nuovo topic, invia il topic attuale al client
                std::string topic = channel->getTopic();
                std::string response = ":YourServer 332 " + client->getNickname() + " " + channelName + " " + topic + "\r\n";
                send(client_fd, response.c_str(), response.length(), 0);
            }
        } else {
            // Il client non è un operatore, invia il topic attuale al client
            if (cmdParams.size() >= 3 && channel->getMode_t()) {
                std::string newTopic = cmdParams[2];
                for (size_t i = 3; i < cmdParams.size(); ++i) {
                    newTopic += " " + cmdParams[i];
                }
                channel->setTopic(newTopic);

                // Invia conferma al client
                std::string response = ":YourServer 332 " + client->getNickname() + " " + channelName + " " + newTopic + "\r\n";
                send(client_fd, response.c_str(), response.length(), 0);
            }
            else if(cmdParams.size() >= 3 && !channel->getMode_t())
            {

            }
            else
            {
                // Il comando non specifica un nuovo topic, invia il topic attuale al client
                std::string topic = channel->getTopic();
                std::string response = ":YourServer 332 " + client->getNickname() + " " + channelName + " " + topic + "\r\n";
                send(client_fd, response.c_str(), response.length(), 0);
            }
        }
    } else {
        // Canale non trovato, invia un messaggio di errore
        std::string errorMsg = ":YourServer 403 " + clients[client_fd]->getNickname() + " " + channelName + " :No such channel\r\n";
        send(client_fd, errorMsg.c_str(), errorMsg.length(), 0);
    }
}
