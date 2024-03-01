#include "Client.hpp"

Client::Client(int fd) : socket_fd(fd), isRegistered(false), hasReceivedNick(false), hasReceivedUser(false), isjoin(0) {
    this->channel = "";
}

void Client::setNickname(const std::string& nick) {
    nickname = nick;
}
void Client::setChannel(std::string chan){
    channel = chan;
}
const std::string& Client::getNickname() const {
    return nickname;
}

const std::string& Client::getChannel() const {
    return channel;
}


void Client::setIsJoin(){
    isjoin++;
}

int Client::getIsJoin() {
    return isjoin;
}

void Client::mergeBuffer(char *client_buffer)
{
    buffer = buffer + client_buffer;
}
void Client::clearBuffer()
{
    buffer.clear();
}
std::string Client::getBuffer()
{
    return buffer;
}