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

void initSocketsError(const char * err, std::vector<int>socketFds, struct addrinfo * addrinfo, bool flagAddrInfo){

    if (flagAddrInfo)
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
            exit(1);
        
        int listen_fd;
        listen_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (listen_fd == -1)
            initSocketsError("socket: ", this->SocketFds, res, true);

        this->SocketFds.push_back(listen_fd);

        int yes = 1;
        if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
            initSocketsError("stesocketopt: ", this->SocketFds, res, true);
        

        if (bind(listen_fd, res->ai_addr, res->ai_addrlen) == 0)
            initSocketsError("bind: ", this->SocketFds, res, true);


        if (listen(listen_fd, SOMAXCONN) < 0)
            initSocketsError("listen: ", this->SocketFds, res, true);
    }

}