#include "../inc/webserv.hpp"
#include "../server/Server.hpp"

Server::Server(ConfigFile & config) : serverConfig(config)/*, httpRequest(config)*/{}

struct addrinfo * initSocketData(const char * host, const char * port){

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

void initSocketsError(const char * err, std::vector<std::pair<int, std::pair<std::string, std::string> > > socketFds, struct addrinfo * addrinfo){

    if (addrinfo)
        freeaddrinfo(addrinfo);

    if (err)
        perror(err);

    for(int i = 0; i < socketFds.size(); i++) close(socketFds[i].first);

    exit(1);
}

void Server::initSockets(){

    struct addrinfo *res;

    for(int i = 0; i < serverConfig.servers.size(); i++){

        if (serverConfig.servers[i].isVirtual)
            continue;
        
        res = initSocketData(serverConfig.servers[i].host.c_str(), serverConfig.servers[i].port.c_str());
        if(res == NULL)
            initSocketsError(NULL, this->SocketFds, NULL);
        
        int socket_fd;
        socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (socket_fd == -1)
            initSocketsError("socket: ", this->SocketFds, res);

        this->SocketFds.push_back(std::make_pair(socket_fd, std::make_pair(serverConfig.servers[i].host, serverConfig.servers[i].port)));

        if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) < 0)
            initSocketsError("fcntl: ", this->SocketFds, res);

        int yes = 1;
        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
            initSocketsError("stesocketopt: ", this->SocketFds, res);
        

        if (bind(socket_fd, res->ai_addr, res->ai_addrlen) < 0)
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

        int listen_fd = SocketFds[i].first;

        struct epoll_event event;
        event.events = EPOLLIN | EPOLLOUT | EPOLLET;
        event.data.fd = listen_fd;

        if (epoll_ctl(this->epollFd, EPOLL_CTL_ADD, listen_fd, &event) == -1) {
           
            close(this->epollFd);
            initSocketsError("epoll_ctl", this->SocketFds, NULL);
        }
    }
}
std::pair<std::string, std::string> getSockInfo(int SocketFd, std::vector<std::pair<int, std::pair<std::string, std::string> > > & vectorinfo){

    for(int i = 0; i < vectorinfo.size(); i++){

        if (SocketFd == vectorinfo[i].first)
            return vectorinfo[i].second;
    }

    return vectorinfo[0].second;
}

bool is_match(int & fd, std::vector<std::pair<int, std::pair<std::string, std::string> > > & fds){

    for(int i = 0; i < fds.size(); i++){

        if (fds[i].first == fd)
            return true;
    }
    return false;
}


void Server::acceptConnections(){

    struct epoll_event events[100];
    // std::vector<std::pair<int, std::pair<std::string, std::string> > > client_fds;

    while (true){
        
        int ready_fds = epoll_wait(this->epollFd, events, 100, -1);
        std::cout << "waiting :" << std::endl;
        if (ready_fds < 0){
            
            close(this->epollFd);
            initSocketsError("epoll_event: ", this->SocketFds, NULL);
        }

        for(int i = 0; i < ready_fds; i++){

            int fd = events[i].data.fd;

            if (is_match(fd, SocketFds)){

                int client_fd = accept(fd, NULL, NULL);
                if (client_fd < 0){

                    close(this->epollFd);
                    initSocketsError("accept: ", this->SocketFds, NULL);
                }

                fcntl(client_fd, F_SETFL, O_NONBLOCK);

                struct epoll_event event;
                event.events = EPOLLIN | EPOLLOUT | EPOLLET;
                event.data.fd = client_fd;

                if (epoll_ctl(this->epollFd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
                    
                    close(this->epollFd);
                    close(client_fd);
                    initSocketsError(NULL, this->client_fds, NULL);
                    initSocketsError("epoll_ctl", this->SocketFds, NULL);
                }

                this->client_fds.push_back(std::make_pair(client_fd, getSockInfo(fd, this->SocketFds)));
                
                client_requests[client_fd].client_fd = client_fd;
                client_requests[client_fd].host = getSockInfo(fd, this->SocketFds).first;
                client_requests[client_fd].port = getSockInfo(fd, this->SocketFds).second;
                continue;
            }
            if (is_match(fd, this->client_fds)){
                // httpRequest.readRequest(fd);
                try{

                    client_requests[fd].readRequest();
                }catch (int e){

                    // handle response
                }

            }
            else{
                std::cout << "no such socket linked to this fd: " << fd << std::endl;
            }        
    
                // close(client_fd);
        }
    }
}
