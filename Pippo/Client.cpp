#include "Client.hpp"

Client::Client(int fd) : socket_fd(fd), isRegistered(false), hasReceivedNick(false), hasReceivedUser(false) {
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
