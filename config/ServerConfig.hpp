#pragma once 

#include "../inc/webserv.hpp"

struct ServerConfig {

    std::string host;
    int port;
    std::vector<std::string> server_names;
    std::map<int, std::string> error_pages;
    size_t client_max_body_size;
    std::vector<LocationConfig> locations;

    bool isDefault; // true for the first server on this host:port
};