#include "Client.hpp"

Client::Client(int fd) : socket_fd(fd), isRegistered(false), hasReceivedNick(false), hasReceivedUser(false) {}

void Client::setNickname(const std::string& nick) {
    nickname = nick;
}

const std::string& Client::getNickname() const {
    return nickname;
}
