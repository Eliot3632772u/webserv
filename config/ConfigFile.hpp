#pragma once

#include "../inc/webserv.hpp"
#include "ServerConfig.hpp"

class ConfigFile
{
	public:
		std::vector<ServerConfig> servers;
		ServerConfig& getServerByServerName(std::string serverName);
};
