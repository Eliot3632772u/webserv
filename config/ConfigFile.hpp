#pragma once 

#include "inc/webserv.hpp"

struct ConfigFile{

    std::vector<ServerConfig> servers;

    void loadConfigFile(std::string filePath);
};