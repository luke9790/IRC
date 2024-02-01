#include "IRCServ.hpp"

IRCServ::IRCServ(int port, const std::string& password) : port(port), password(password) {
    // Create a socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        throw std::runtime_error("Socket creation failed");
    }

    // Set the socket to non-blocking mode
    fcntl(server_fd, F_SETFL, O_NONBLOCK);

    // Address structure
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        close(server_fd);
        throw std::runtime_error("Socket bind failed");
    }

    // Listen for connections
    if (listen(server_fd, SOMAXCONN) < 0) {
        close(server_fd);
        throw std::runtime_error("Socket listen failed");
    }
}

IRCServ::~IRCServ() {
    // Iterate through and delete Client objects
    std::map<int, Client*>::iterator it;
    for (it = clients.begin(); it != clients.end(); ++it) {
        delete it->second;
    }

    // Iterate through and delete Channel objects
    std::map<std::string, Channel*>::iterator ch_it;
    for (ch_it = channels.begin(); ch_it != channels.end(); ++ch_it) {
        delete ch_it->second;
    }
}

void IRCServ::run() {
    fd_set master_set, read_fds;
    int max_fd;

    FD_ZERO(&master_set);
    FD_SET(server_fd, &master_set);
    max_fd = server_fd;

    while (true) {
        read_fds = master_set;

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            throw std::runtime_error("Select error");
        }

        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == server_fd) {
                    // Handle new connections
                    struct sockaddr_in client_addr;
                    socklen_t client_addr_len = sizeof(client_addr);
                    int new_client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

                    if (new_client_fd == -1) {
                        // Handle accept error
                    } else {
                        // Set the new socket to non-blocking mode
                        fcntl(new_client_fd, F_SETFL, O_NONBLOCK);
                        // Add new client to the map
                        clients[new_client_fd] = new Client(new_client_fd);
                        if (new_client_fd > max_fd) {
                            max_fd = new_client_fd;
                        }
                    }
                } else {
                    // Handle data from clients
                    char buffer[1024];
                    int nbytes = recv(i, buffer, sizeof(buffer), 0);

                    if (nbytes <= 0) {
                        // Handle disconnect or error
                        delete clients[i];
                        clients.erase(i);
                        close(i);
                        FD_CLR(i, &master_set);
                    } else {
                        // Process the received data as an IRC command
                        // This involves parsing the buffer and acting on the command
                    }
                }
            }
        }
    }
}
