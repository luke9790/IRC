#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include "IRCServ.hpp"

bool keepRunning = true;

void signalHandler(int signum) {
    std::cout << "Interruzione rilevata, inizio procedura di pulizia..." << std::endl;
	(void)signum;
	keepRunning = false;
}


int main(int ac, char* av[])
{
    if (ac != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    int port = std::atoi(av[1]);
    std::string password = av[2];

    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = signalHandler;
    sigaction(SIGINT, &action, NULL);

 try {
        IRCServ server(port, password);
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
