#include "IRCServ.hpp"
#include "Handler.hpp"

IRCServ::IRCServ(int port, const std::string& password) : port(port), password(password) {
    // Create a socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        throw std::runtime_error("Socket creation failed");
    }

    // Set the socket to non-blocking mode
    fcntl(server_fd, F_SETFL, O_NONBLOCK); // funzione per modificare le proprieta' del socket

    // Address structure
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address)); // usata per inizializzare tutti i bit a zero ed evitare problemi per il bind
    address.sin_family = AF_INET; // setta la famiglia di indirizzi della socket a ipv4
    address.sin_addr.s_addr = INADDR_ANY; // settiamo l'address in modo che accetti tutti gli indirizzi ip della macchina
    address.sin_port = htons(port); // imposta la porta su cui la socket deve ascoltare

    // Bind the socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) { // associamo la socket alla struttura precedente (definiamo le connessioni e la rete)
        close(server_fd);
        throw std::runtime_error("Socket bind failed");
    }

    // Listen for connections
    if (listen(server_fd, SOMAXCONN) < 0) { // la socket ascolta le connessioni in entrata entro un numero massimo definito da somaxconn
        close(server_fd);
        throw std::runtime_error("Socket listen failed");
    }
    // Inizializza i canali predefiniti
    channels["generale"] = new Channel("generale");
    channels["programmazione"] = new Channel("programmazione");
    // Se desideri, puoi anche impostare topic predefiniti qui
    channels["generale"]->setTopic("Benvenuti nel canale generale");
    channels["programmazione"]->setTopic("Benvenuti nel canale di programmazione");
}

IRCServ::~IRCServ() {
    std::map<int, Client*>::iterator it;
    for (it = clients.begin(); it != clients.end(); ++it) {
        delete it->second;
    }

    std::map<std::string, Channel*>::iterator ch_it;
    for (ch_it = channels.begin(); ch_it != channels.end(); ++ch_it) {
        delete ch_it->second;
    }
}


void IRCServ::run() {
    fd_set master_set, read_fds; // creiamo due set di file descriptor, uno master che tiene traccia di tutto, uno per i socket da leggere
    int max_fd;

    FD_ZERO(&master_set); // settiamo tutto a zero(cioe' niente da leggere)
    FD_SET(server_fd, &master_set); // aggiungiamo il socket al master set
    max_fd = server_fd; // ci serve per tenere traccia del fd piu' alto (necessario per select)

    while (true) 
    {
        read_fds = master_set; // copiamo il master in read perche' select modifica i valori

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) { // cerca nel read se ci sono fd pronti da leggere
            throw std::runtime_error("Select error");
        }

        for (int i = 0; i <= max_fd; i++)
        {
            if (FD_ISSET(i, &read_fds))
            {
                if (i == server_fd) // significa che ci sono nuove connessioni in entrata
                { // Handle new connections
                    struct sockaddr_in client_addr;
                    socklen_t client_addr_len = sizeof(client_addr);
                    int new_client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len); // una volta creata la struttura come in precedenza, accettiamo la connessione
                    // una volta accetta la connessione, dobbiamo rifare i passaggi precedenti e aggiungere il Client alla nostra mappa del server
                    if (new_client_fd == -1) 
                    {
                         std::cerr << "Error accepting connection: " << strerror(errno) << std::endl; // DA VALUTARE SE SUFFICIENTE
                    } 
                    else 
                    {
                        fcntl(new_client_fd, F_SETFL, O_NONBLOCK); // Set to non-blocking
                        clients[new_client_fd] = new Client(new_client_fd); // Add to clients map
                        FD_SET(new_client_fd, &master_set); // Add to master set
                        if (new_client_fd > max_fd)
                            max_fd = new_client_fd; // Update max if needed
                    }
                }
                else // significa che abbiamo dati da un client esistente
                {
                    // Handle data from clients
                    char buffer[1024];
                    int nbytes = recv(i, buffer, sizeof(buffer), 0); // leggiamo i dati

                    if (nbytes <= 0) // errore o disconnessione del client
                    {
                        close(i); // Close the socket
                        FD_CLR(i, &master_set); // Remove from master set
                        delete clients[i]; // Cleanup
                        clients.erase(i); // Remove from clients map
                    }
                    else // abbiamo un comando da processare
                    {
                        buffer[nbytes] = '\0'; // Null-terminate what we received and process
                        // Parse the command from the buffer
                        std::vector<std::string> cmdParams = CommandParser::parseCommand(std::string(buffer));
                        int actionRequired = Handler::handleCommand(i, cmdParams, clients, channels);
                        if (actionRequired == 1) {
                            // Il client ha inviato il comando QUIT
                            close(i); // Chiude il socket
                            FD_CLR(i, &master_set); // Rimuove dal master_set
                            delete clients[i]; // Dealloca l'oggetto Client
                            clients.erase(i); // Rimuove dalla mappa dei client
                            continue; // Vai al prossimo ciclo del loop
                        }
                    }
                }
            }
        }
    }
}
