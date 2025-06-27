#pragma once

#include "../inc/webserv.hpp"
#include "../config/ServerConfig.hpp"

struct Server {

    std::vector<ServerConfig> & servers;
    std::vector<int> SocketFds;
    int epollFd;

    Server(std::vector<ServerConfig> & servers);
    void initSockets();
    void watchReadySockets();
    void acceptConnections();
};