#pragma once

#include "../inc/webserv.hpp"
#include "../config/ServerConfig.hpp"
#include "../httpRequest/httpRequest.hpp"
 #include "../config/ConfigFile.hpp"

struct Server {

    ConfigFile & serverConfig;
    // std::vector<int> SocketFds;// change this to map<sockfd, {host, port}>
    // std::map<int, std::pair<std::string, std::string>> SocketFds 
    std::vector<std::pair<int, std::pair<std::string, std::string> > > SocketFds;
    std::vector<std::pair<int, std::pair<std::string, std::string> > > client_fds;
    int epollFd;
    std::map<int, HttpRequest> clients;
    // HttpRequest  httpRequest;
    std::map<int, HttpRequest> client_requests;


    Server(ConfigFile & servers);
    void initSockets();
    void watchReadySockets();
    void acceptConnections();
    // std::pair<std::string, std::string> getSockInfo(int SocketFd);
    // void printData(int & fd, std::vector<std::pair<int, std::pair<std::string, std::string> > > & fd_info);
};
