#include "Channel.hpp"

Channel::Channel(std::string channelName) : topic("") {
    // Inizializzazione di altre proprietà, se necessario
}

Channel::~Channel() {
    // Pulizia delle risorse, se necessario 
    // poiché stiamo usando solo puntatori a Client già gestiti altrove, non dobbiamo deallocare gli oggetti Client qui
}


void Channel::addClient(Client* client) {
    // Verifica che il client non sia già nel canale
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i] == client) {
            return; // Il client è già nel canale
        }
    }
    clients.push_back(client); // Aggiungi il client al canale
}

void Channel::removeClient(Client* client) {
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (*it == client) {
            clients.erase(it); // Rimuovi il client dal canale
            break; // Uscita dopo la rimozione per evitare problemi di iterazione
        }
    }
}


void Channel::setTopic(const std::string& topic) {
    this->topic = topic;
}

const std::string& Channel::getTopic() const {
    return topic;
}

std::vector<Client*> Channel::getClients() const {
    return clients;
}