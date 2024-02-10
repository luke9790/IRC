#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080); // Replace YourPortNumber with your server's port
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return 1;
    }

    std::string message = "Hello Server\n";
    send(sock, message.c_str(), message.length(), 0);

    char buffer[1024] = {0};
    read(sock, buffer, 1024);
    std::cout << "Message from server: " << buffer << std::endl;

    close(sock);
    return 0;
}
