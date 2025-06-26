#pragma once

#include "../inc/webserv.hpp"

struct Server {

    std::vector<ServerConfig> & servers;
    std::vector<int> SocketFds;

    Server(std::vector<ServerConfig> & servers);
    void initSockets();
};