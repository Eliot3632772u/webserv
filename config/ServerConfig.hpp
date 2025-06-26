#pragma once 

#include "../inc/webserv.hpp"

struct ServerConfig {

    bool isVirtualServer; // is the server host and port are not duplicates 
    std::string host;
    std::string port;
    std::vector<std::string> server_names;
    std::map<int, std::string> error_pages;
    size_t client_max_body_size;
    std::vector<LocationConfig> locations;

    bool isDefault; // true for the first server on this host:port

};