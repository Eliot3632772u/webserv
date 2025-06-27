#pragma once 

#include "../inc/webserv.hpp"
#include "LocationConfig.hpp"

class ServerConfig
{
	public:
		bool isVirtual;
		bool isDefault;
		std::string host;
		std::string port;
		std::vector<std::string> server_names;
		std::map<int, std::string> error_pages;
		size_t max_body_size;
		std::vector<LocationConfig> locations;
};
