/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmasetti <lmasetti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/21 14:25:37 by lmasetti          #+#    #+#             */
/*   Updated: 2024/03/21 14:25:38 by lmasetti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>

std::string Channel::getCurrentModes() const {
    std::string modes = "+"; // Inizia sempre con un segno + per indicare le modalità attive
    std::stringstream ss;

    if (topic_mode) {
        modes += "t"; // Il topic del canale può essere modificato solo dagli operatori
    }
    if (invite_only) {
        modes += "i"; // Il canale è impostato come solo su invito
    }
    if (user_limits > 0) {
        modes += "l"; // Esiste un limite di utenti per il canale
    }
    if (!pwd.empty()) {
        modes += "k"; // Il canale è protetto da password
    }

    // Aggiungi i parametri delle modalità dopo le lettere delle modalità, se necessario
    if (user_limits > 0) {
        ss << user_limits;
        modes += " " + ss.str(); // Converte il numero in stringa e lo aggiunge
    }
    return modes;
}


void Channel::inviteClient(int client_fd) {
    // Verifica prima se il client è già stato invitato
    if (std::find(invitedClients.begin(), invitedClients.end(), client_fd) == invitedClients.end()) {
        invitedClients.push_back(client_fd); // Invita il client se non è già nella lista
    }
}

void Channel::unsetOperator(int client_fd) {
    std::vector<int>::iterator it = std::find(operators.begin(), operators.end(), client_fd);
    if (*it == client_fd) {
        operators.erase(it);
    }
}


bool Channel::isInvited(int client_fd) const {
    return std::find(invitedClients.begin(), invitedClients.end(), client_fd) != invitedClients.end();
}

void Channel::removeInvitedClient(int client_fd) {
    // Cerca l'ID del client nel vettore degli invitati
    std::vector<int>::iterator it = std::find(invitedClients.begin(), invitedClients.end(), client_fd);
    if (it != invitedClients.end()) {
        invitedClients.erase(it);
    }
}

Channel::Channel(std::string channelName, int fd) : name(channelName), topic(""), userCount(0), topic_mode(false), user_limits(0), invite_only(false), pwd(""){
    setChannelOperator(fd);
}

void Channel::setChannelOperator(int fd_op)
{
    operators.push_back(fd_op);
}

void Channel::removeChannelOperator(int fd_op)
{
    std::vector<int>::iterator it;
    for(it = operators.begin(); it != operators.end(); it++)
    {
        if (*it == fd_op)
        {
            it = operators.erase(it);
            break;
        }
    }
}

Channel::~Channel() {}

void Channel::addClient(Client* client) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i] == client) {
            return; // Il client è già nel canale
        }
    }
    client->channels.push_back(this->name);
    userCount++;
    clients.push_back(client); // Aggiungi il client al canale
}

void Channel::removeClient(Client* client) {
    // Ricerca e rimozione del client dalla lista dei clienti del canale
    std::vector<Client*>::iterator it = std::find(clients.begin(), clients.end(), client);
    if (it != clients.end()) {
        clients.erase(it);
        userCount--;

        std::vector<std::string>::iterator it2 = std::find(client->channels.begin(), client->channels.end(), this->name);
        if (it2 != client->channels.end()) {
            client->channels.erase(it2);
        }
    }
}




bool Channel::isOperator(int client_fd)
{
    for (std::vector<int>::iterator it = operators.begin(); it != operators.end(); ++it) {
        if (client_fd == *it)
            return true;
    }
    return false;
}

bool Channel::isClientInChannel(int client_fd) const {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i]->socket_fd == client_fd) {
            return true; // Il client è presente nel canale
        }
    }
    return false; // Il client non è stato trovato nel canale
}

void Channel::broadcast(const std::string& message) {
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        Client* client = *it;
        // Controlla se il client corrente deve essere escluso dalla broadcast
        if (client->socket_fd != -1) {
            send(client->socket_fd, message.c_str(), message.length(), 0);
        }
    }
}

void Channel::setName(const std::string& name) {
    this->name = name;
}

const std::string& Channel::getName() const {
    return name;
}

void Channel::setTopic(const std::string& topic) {
    this->topic = topic;
}

const std::string& Channel::getTopic() const {
    return topic;
}

std::vector<Client*> Channel::getClients() const{
    return clients;
}

int Channel::getUserCount() const
{
    return userCount;
}

void Channel::setMode_t(bool flg)
{
    topic_mode = flg;
}

void Channel::setInviteOnly(bool value)
{
    invite_only = value;
}

void Channel::setUserLimits(int limits)
{
    user_limits = limits;
}

void Channel::setPassword(std::string pass)
{
    pwd = pass;
}

int Channel::getUserLimits()
{
    return user_limits;
}

std::string Channel::getPassword()
{
    return pwd;
}

bool Channel::getInviteOnly()
{
    return invite_only;
}

bool Channel::getMode_t()
{
    return topic_mode;
}
