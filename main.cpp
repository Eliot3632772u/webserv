#include "inc/webserv.hpp"

int main(){

    ServerConfig serverconf;

    serverconf.port = "8080";
    serverconf.host = "127.0.0.1";
    serverconf.isVirtualServer = false;

    ConfigFile config;

    config.servers.push_back(serverconf);

    Server server(config.servers);

    server.initSockets();
    server.watchReadySockets();
    server.acceptConnections();
}