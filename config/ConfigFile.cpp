#include "ConfigFile.hpp"

// function should look up with serverName and host/port as well.
ServerConfig& ConfigFile::getServerByServerName(std::string serverName)
{
	for (size_t i = 0; i < this->servers.size(); i++)
	{
		for (size_t j = 0; j < this->servers[i].server_names.size(); j++)
		{
			if (serverName == this->servers[i].server_names[j])
				return this->servers[i];
		}
	}

	// for (size_t i = 0; i < this->servers.size(); i++)
	// {
	// 	if (this->servers[i].isDefault)
	// 		return servers[i];
	// }

	throw std::runtime_error("Could not locate a target servername: " + serverName);
}