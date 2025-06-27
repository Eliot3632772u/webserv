#include "inc/webserv.hpp"
#include "config/ServerConfig.hpp"
#include "config/LocationConfig.hpp"
#include "config/ConfigFile.hpp"
#include "config/ConfigParser.hpp"
#include "server/Server.hpp"

int main(int ac, char **av)
{
	if (ac != 2)
		return 1;

	ConfigParser parser;
	try
	{
		parser.parseConfigFile(av[1]);
		parser.validateConfigs();
		// parser.showConfigs();
	}
	catch(std::exception& err)
	{
		std::cerr << err.what() << std::endl;
		return 1;
	}
	
    // ServerConfig serverconf;
    // serverconf.port = "8080";
    // serverconf.host = "127.0.0.1";
    // serverconf.isVirtual = false;

    ConfigFile& config = parser.getConfigs();

    // config.servers.push_back(serverconf);

    Server server(config.servers);

    server.initSockets();
    server.watchReadySockets();
    server.acceptConnections();
}


// 	ConfigFile& configs = parser.getConfigs();
// 	ServerConfig& serv = configs.getServerByServerName("tester");
// 	std::cout << serv.host << "/" << serv.port << std::endl;