#include "Client.hpp"

Client::Client(int fd) : socket_fd(fd) {}

void Client::setNickname(const std::string& nick) {
    nickname = nick;
}

const std::string& Client::getNickname() const {
    return nickname;
}
