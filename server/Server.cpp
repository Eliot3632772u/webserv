#include "../inc/webserv.hpp"

Server::Server(std::vector<ServerConfig> & servers) : servers(servers){}

struct addrinfo * initSockerData(const char * host, const char * port){

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(host, port, &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(status) << "\n";
        return NULL;
    }

    return res;
}

void initSocketsError(const char * err, std::vector<int>socketFds, struct addrinfo * addrinfo){

    if (addrinfo)
        freeaddrinfo(addrinfo);

    if (err)
        perror(err);

    for(int i = 0; i < socketFds.size(); i++) close(socketFds[i]);

    exit(1);
}

void Server::initSockets(){

    struct addrinfo *res;

    for(int i = 0; i < servers.size(); i++){

        if (servers[i].isVirtualServer)
            continue;
        
        res = initSockerData(servers[i].host.c_str(), servers[i].port.c_str());
        if(res == NULL)
            initSocketsError(NULL, this->SocketFds, NULL);
        
        int socket_fd;
        socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (socket_fd == -1)
            initSocketsError("socket: ", this->SocketFds, res);

        this->SocketFds.push_back(socket_fd);

        if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) < 0)
            initSocketsError("fcntl: ", this->SocketFds, res);

        int yes = 1;
        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
            initSocketsError("stesocketopt: ", this->SocketFds, res);
        

        if (bind(socket_fd, res->ai_addr, res->ai_addrlen) == 0)
            initSocketsError("bind: ", this->SocketFds, res);


        if (listen(socket_fd, SOMAXCONN) < 0)
            initSocketsError("listen: ", this->SocketFds, res);

        freeaddrinfo(res);
    }
}

void Server::watchReadySockets(){

    this->epollFd = epoll_create1(0);
    if (this->epollFd < 0)
        initSocketsError("epoll_create: ", this->SocketFds, NULL);

    for (size_t i = 0; i < SocketFds.size(); ++i) {

        int listen_fd = SocketFds[i];

        struct epoll_event event;
        event.events = EPOLLIN | EPOLLET;
        event.data.fd = listen_fd;

        if (epoll_ctl(this->epollFd, EPOLL_CTL_ADD, listen_fd, &event) == -1) {
           
            close(this->epollFd);
            initSocketsError("epoll_ctl", this->SocketFds, NULL);
        }
    }
}

void Server::acceptConnections(){

    struct epoll_event events[100];
    while (true){

        int ready_fds = epoll_wait(this->epollFd, events, 100, -1);
        if (ready_fds < 0){

            close(this->epollFd);
            initSocketsError("epoll_event: ", this->SocketFds, NULL);
        }

        for (int i = 0; i < ready_fds; i++){

            int sock_fd = events[i].data.fd;
            if (std::find(this->SocketFds.begin(), this->SocketFds.end(), sock_fd) != this->SocketFds.end()){

                int client_fd = accept(sock_fd, NULL, NULL);
                if (client_fd < 0){

                    close(this->epollFd);
                    initSocketsError("accept: ", this->SocketFds, NULL);
                }

                // parse request
                // send response

                close(client_fd);
            }
            else{
                std::cout << "no such socket linked to this fd: " << sock_fd << std::endl;
            }
        }
    }
}