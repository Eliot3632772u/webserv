#include "ConfigParser.hpp"

ConfigParser::ConfigParser()
{
	this->server_index = 0;
	this->line = "none";
}

ConfigFile& ConfigParser::getConfigs()
{
	return this->configs;
}

void ConfigParser::validateConfigs()
{
	std::map<std::string, std::vector<size_t> > hostPortMap;
	for (size_t i = 0; i < this->configs.servers.size(); i++)
	{
		const ServerConfig& server = configs.servers[i];
		std::string key = server.host + ":" + server.port;
		hostPortMap[key].push_back(i);
	}

	std::map<std::string, std::vector<size_t> >::iterator groupIt;
	for (groupIt = hostPortMap.begin(); groupIt != hostPortMap.end(); groupIt++)
	{
		std::vector<size_t>& indices = groupIt->second;
		std::set<std::string> usedServerNames;

		if (indices.empty())
			continue;
		
		ServerConfig& first = configs.servers[indices[0]];
		first.isDefault = true;
		first.isVirtual = false;

		std::vector<std::string>::const_iterator nameIt;
		for (nameIt = first.server_names.begin(); nameIt != first.server_names.end(); nameIt++)
		{
			if (!usedServerNames.insert(*nameIt).second)
				throw std::runtime_error("Duplicate server_name: " + *nameIt);
		}

		for (size_t j = 1; j < indices.size(); j++)
		{
			ServerConfig& current = configs.servers[indices[j]];
			current.isDefault = false;
			current.isVirtual = true;

			for (nameIt = current.server_names.begin(); nameIt != current.server_names.end(); nameIt++)
			{
				if (usedServerNames.find(*nameIt) != usedServerNames.end())
					throw std::runtime_error("Duplicate server_name: " + *nameIt);
			}

			for (nameIt = current.server_names.begin(); nameIt != current.server_names.end(); nameIt++)
				usedServerNames.insert(*nameIt);
		}
	}

	for (size_t i = 0; i < configs.servers.size(); i++)
	{
		const ServerConfig& a = configs.servers[i];
		for (size_t j = i + 1; j < configs.servers.size(); j++)
		{
			const ServerConfig& b = configs.servers[j];
			if (a.host == b.host && a.port == b.port && a.server_names == b.server_names)
				throw std::runtime_error("Duplicate server detected(host, port, servernames)");
		}
	}
}

void ConfigParser::processPath()
{
	size_t pos = this->line.find("=") + 1;
	while (pos < this->line.size() && this->line[pos] == ' ')
		pos++;
	std::string path = this->line.substr(pos);
	size_t end = path.find_last_not_of(" ");
	if (end != std::string::npos)
		path = path.substr(0, end + 1);
	else
		path.clear();
	if (path.empty())
		throw std::runtime_error("Location path cannot be empty");
	pos = 0;
	for (; pos < path.size(); pos++)
	{
		char c = path[pos];
		if (!(std::isalnum(c) || c == '/' || c == '_' || c == '-' || c == '~' || c == '.'))
			throw std::runtime_error("Invalid location path character");
	}
	pos = 0;
	while ((pos = path.find("//", pos)) != std::string::npos)
		path.replace(pos, 2, "/");
	this->configs.servers[this->server_index].locations.back().path = path;
}

void ConfigParser::processRoot()
{
	size_t pos = this->line.find("=") + 1;
	while (pos < this->line.size() && this->line[pos] == ' ')
		pos++;
	std::string path = this->line.substr(pos);
	size_t end = path.find_last_not_of(" ");
	if (end != std::string::npos)
		path = path.substr(0, end + 1);
	else
		path.clear();
	if (path.empty())
		throw std::runtime_error("Location root cannot be empty");
	pos = 0;
	for (; pos < path.size(); pos++)
	{
		char c = path[pos];
		if (!(std::isalnum(c) || c == '/' || c == '_' || c == '-' || c == '~' || c == '.'))
			throw std::runtime_error("Invalid location root character");
	}
	pos = 0;
	while ((pos = path.find("//", pos)) != std::string::npos)
		path.replace(pos, 2, "/");
	this->configs.servers[this->server_index].locations.back().root = path;
}

void ConfigParser::processMethods()
{
	size_t pos = this->line.find("=") + 1;
	while (pos < this->line.size() && this->line[pos] == ' ')
		pos++;
	std::string methods = this->line.substr(pos);
	size_t end = methods.find_last_not_of(" ");
	if (end != std::string::npos)
		methods = methods.substr(0, end + 1);
	else
		methods.clear();
	if (methods.empty())
		throw std::runtime_error("One method is required");
	std::istringstream iss(methods);
	std::string method;
	while (iss >> method)
	{
		if (method != "GET" && method != "POST" && method != "DELETE")
			throw std::runtime_error("Unsupported HTTP method in location block");
		this->configs.servers[this->server_index].locations.back().methods.insert(method);
	}
}

void ConfigParser::processRedirect()
{
	size_t pos = this->line.find("=") + 1;
	while (pos < this->line.size() && this->line[pos] == ' ')
		pos++;
	std::string redirect = this->line.substr(pos);
	size_t end = redirect.find_last_not_of(" ");
	if (end != std::string::npos)
		redirect = redirect.substr(0, end + 1);
	else
		redirect.clear();
	if (redirect.empty())
		throw std::runtime_error("Redirect once defined cannot be empty");
	int code;
	std::string path;
	std::istringstream iss(redirect);
	iss >> code >> path;
	if (!iss.eof() || iss.fail())
		throw std::runtime_error("Invalid redirect directive in locaiton");
	if (code < 300 || code > 308)
		throw std::runtime_error("Invalid redirect code");
	else if (path[0] == '/')
	{
		for (size_t i = 0; i < path.size(); i++)
		{
			char c = path[i];
			if (!(std::isalnum(c) || c == '/' || c == '_' || c == '-' || c == '~' || c == '.'))
				throw std::runtime_error("Invalid location path character");
		}
		size_t pos = 0;
		while ((pos = path.find("//", pos)) != std::string::npos)
			path.replace(pos, 2, "/");
	}
	else if (!(path.substr(0, 7) == "http://" && path.size() > 7))
		throw std::runtime_error("Invalid redirect path in location");
	this->configs.servers[this->server_index].locations.back().redirect[code] = path;
}

void ConfigParser::processDirectoryListing()
{
	size_t pos = this->line.find("=") + 1;
	while (pos < this->line.size() && this->line[pos] == ' ')
		pos++;
	std::string listing = this->line.substr(pos);
	size_t end = listing.find_last_not_of(" ");
	if (end != std::string::npos)
		listing = listing.substr(0, end + 1);
	else
		listing.clear();
	if (listing.empty())
		throw std::runtime_error("Directory listing once defined cannot be empty");
	if (listing == "true")
		this->configs.servers[this->server_index].locations.back().directory_listing = true;
	else if (listing == "false")
		this->configs.servers[this->server_index].locations.back().directory_listing = false;
	else
		throw std::runtime_error("directory_listing in location can be either 'true' or 'false'");
}

void ConfigParser::processDefaultFile()
{
	size_t pos = this->line.find("=") + 1;
	while (pos < this->line.size() && this->line[pos] == ' ')
		pos++;
	std::string value = this->line.substr(pos);
	size_t end = value.find_last_not_of(" ");
	if (end != std::string::npos)
		value = value.substr(0, end + 1);
	else
		value.clear();
	if (value.empty())
		throw std::runtime_error("Default file once defined cannot be empty");
	for (size_t i = 0; i < value.size(); i++)
	{
		char c = value[i];
		if (!(std::isalnum(c) || c == '/' || c == '_' || c == '-' || c == '~' || c == '.'))
			throw std::runtime_error("Invalid location default file character");
	}
	this->configs.servers[this->server_index].locations.back().default_file = value;
}

void ConfigParser::processUpload()
{
	size_t pos = this->line.find("=") + 1;
	while (pos < this->line.size() && this->line[pos] == ' ')
		pos++;
	std::string value = this->line.substr(pos);
	size_t end = value.find_last_not_of(" ");
	if (end != std::string::npos)
		value = value.substr(0, end + 1);
	else
		value.clear();
	if (value.empty())
		throw std::runtime_error("Upload path once defined cannot be empty");
	if (value[0] != '/')
		throw std::runtime_error("Upload path must be absolute path");
	for (size_t i = 0; i < value.size(); i++)
	{
		char c = value[i];
		if (!(std::isalnum(c) || c == '/' || c == '_' || c == '-' || c == '~' || c == '.'))
			throw std::runtime_error("Invalid location default file character");
	}
	pos = 0;
	while ((pos = value.find("//", pos)) != std::string::npos)
		value.replace(pos, 2, "/");
	this->configs.servers[this->server_index].locations.back().upload_store = value;
}

void ConfigParser::processCGI()
{
	size_t pos = this->line.find("=") + 1;
	while (pos < this->line.size() && this->line[pos] == ' ')
		pos++;
	std::string value = this->line.substr(pos);
	size_t end = value.find_last_not_of(" ");
	if (end != std::string::npos)
		value = value.substr(0, end + 1);
	else
		value.clear();
	if (value.empty())
		throw std::runtime_error("CGI once defined cannot be empty");
	std::string ext;
	std::string path;
	std::istringstream iss(value);
	iss >> ext >> path;
	if (!iss.eof() || iss.fail())
		throw std::runtime_error("CGI misconfigured; requires: extention, space, path");
	if (ext != ".php" && ext != ".py" && ext != ".sh")
		throw std::runtime_error("Unsupported CGI");
	if (path[0] != '/')
		throw std::runtime_error("CGI path must absolute");
	for (size_t i = 0; i < path.size(); i++)
	{
		char c = path[i];
		if (!(std::isalnum(c) || c == '/' || c == '_' || c == '-' || c == '~' || c == '.'))
			throw std::runtime_error("Invalid location CGI path character");
	}
	pos = 0;
	while ((pos = path.find("//", pos)) != std::string::npos)
		path.replace(pos, 2, "/");
	this->configs.servers[this->server_index].locations.back().cgi[ext] = path;
}


void ConfigParser::parseLocationBlock()
{
	this->configs.servers[this->server_index].locations.push_back(LocationConfig());
	while (std::getline(this->ifs, this->line))
	{
		if (line.empty())
			continue;
		if (this->line.substr(0, 6) == "path =" || this->line.substr(0, 5) == "path=")
			processPath();
		else if (this->line.substr(0, 6) == "root =" || this->line.substr(0, 5) == "root=")
			processRoot();
		else if (this->line.substr(0, 9) == "methods =" || this->line.substr(0, 8) == "methods=")
			processMethods();
		else if (this->line.substr(0, 10) == "redirect =" || this->line.substr(0, 9) == "redirect=")
			processRedirect();
		else if (this->line.substr(0, 19) == "directory_listing =" || this->line.substr(0, 18) == "directory_listing=")
			processDirectoryListing();
		else if (this->line.substr(0, 14) == "default_file =" || this->line.substr(0, 13) == "default_file=")
			processDefaultFile();
		else if (this->line.substr(0, 14) == "upload_store =" || this->line.substr(0, 13) == "upload_store=")
			processUpload();
		else if (this->line.substr(0, 5) == "cgi =" || this->line.substr(0, 4) == "cgi=")
			processCGI();
		else if (this->line == "[location]" || this->line == "[server]")
			break ;
		else
		{
			std::cout << line << std::endl;
			throw std::runtime_error("Unknown directive in location section!");
		}
	}
}

ConfigFile ConfigParser::parseConfigFile(const char* fileName)
{
	if (fileName == NULL)
		throw std::runtime_error("Config file not supplied!");
	
	this->ifs.open(fileName);
	if (!ifs)
		throw std::runtime_error("Could not read config file!");

	while (this->line == "[server]" || std::getline(this->ifs, this->line))
	while (true)
	{
		if (this->line != "[server]" && !std::getline(this->ifs, this->line))
			break ;
		if (this->line.empty())
			continue;
		else if (this->line == "[server]")
		{
			parseServerBlock();
			while (true)
			{
				if (this->line != "[location]")
					break;
				if (!std::getline(this->ifs, this->line))
					break;
				if (this->line == "[server]")
					break;
				parseLocationBlock();
			}
			this->server_index++;
		}
		else
		{
			std::cout << this->line << std::endl;
			throw std::runtime_error("Syntax error with config file!");
		}
	}
	return this->configs;
}

void ConfigParser::processHost()
{
	std::string::size_type i = this->line.find("=") + 1;
	if (this->line[i] == ' ')
		i++;
	if (i == line.size())
		throw std::runtime_error("Host might be empty!");
	std::string::size_type j = i;
	for (; i < this->line.size(); i++)
	{
		if (!std::isdigit(this->line[i]) && this->line[i] != '.')
			throw std::runtime_error("Issues with host(IPv4)!");
	}
	if (i == j || i - j < 7)
		throw std::runtime_error("host is too small to be true");
	this->configs.servers[this->server_index].host = this->line.substr(j);
}

void ConfigParser::processPort()
{
	std::string::size_type i = this->line.find("=") + 1;
	if (this->line[i] == ' ')
		i++;
	if (i == line.size())
		throw std::runtime_error("Port might be empty!");
	std::string::size_type j = i;
	for (; i < this->line.size(); i++)
	{
		if (!std::isdigit(line[i]))
			throw std::runtime_error("Port has non-digits!");
	}
	this->configs.servers[this->server_index].port = this->line.substr(j);
	if (this->configs.servers[this->server_index].port.size() > 5)
		throw std::runtime_error("Port may not be more than more than 5 digits");
}

void ConfigParser::processServerName()
{
	std::string::size_type i = this->line.find("=") + 1;
	if (this->line[i] == ' ')
		i++;
	if (i == line.size())
		throw std::runtime_error("ServerName might be empty!");
	std::string::size_type j = i;
	for (; i < this->line.size(); i++)
	{
		if (line[i] == '-' && i == j)
			throw std::runtime_error("Servername cannot start with hypher!");
		else if (!(std::isalnum(line[i]) || line[i] == '-' || line[i] == '.'))
			throw std::runtime_error("Servername has invalid characters!");
	}
	this->configs.servers[this->server_index].server_names.push_back(this->line.substr(j));
	if ((this->configs.servers[this->server_index].server_names.end() - 1)->size() > 63)
		throw std::runtime_error("Servername may not be more than more than 63 characters");
}

void ConfigParser::processBodySize()
{
	std::string::size_type i = this->line.find("=") + 1;
	if (this->line[i] == ' ')
		i++;
	if (i == line.size())
		throw std::runtime_error("Max_body_size might be empty!");
	size_t max;
	std::istringstream iss(line.substr(i));
	iss >> max;
	if (!iss.eof() || iss.fail())
		throw std::runtime_error("Issue with max body size!");
	this->configs.servers[this->server_index].max_body_size = max;
}

void ConfigParser::processErrorPage()
{
	std::string::size_type i = this->line.find("=") + 1;
	if (this->line[i] == ' ')
		i++;
	if (i == line.size())
		throw std::runtime_error("Errorpage might be empty!");
	std::string::size_type j = i;
	for (; i < this->line.size(); i++)
	{
		if (i - j == 3)
			break;
		if (!std::isdigit(this->line[i]))
		{
			throw std::runtime_error("Error code can only have digits");
		}
	}
	if (i - j != 3)
		throw std::runtime_error("Error code must be 3 digits");
	int error_code;
	std::istringstream iss(this->line.substr(j, 3));
	iss >> error_code;
	if (!iss.eof() || iss.fail())
		throw std::runtime_error("Erorr status code.");
	if (this->line[i] == ' ')
		i++;
	j = i;
	if (i == this->line.size())
		throw std::runtime_error("Missing path for error page");
	for (; i < this->line.size(); i++)
	{
		if (!(std::isalnum(this->line[i]) 
			|| this->line[i] == '/' 
			|| this->line[i] == '.' 
			|| this->line[i] == '_' 
			|| this->line[i] == '-' 
			|| this->line[i] == '+'))
			{
				std::cout << "'" << this->line[i] << "'" << std::endl;
				throw std::runtime_error("Invalid error page path");

			}
	}
	this->configs.servers[this->server_index].error_pages[error_code] = this->line.substr(j);
}

void ConfigParser::parseServerBlock()
{
	this->configs.servers.push_back(ServerConfig());
	while (std::getline(this->ifs, this->line))
	{
		if (line.empty())
			continue;
		if (this->line.substr(0, 6) == "host =" || this->line.substr(0, 5) == "host=")
			processHost();
		else if (this->line.substr(0, 6) == "port =" || this->line.substr(0, 5) == "port=")
			processPort();
		else if (this->line.substr(0, 13) == "server_name =" || this->line.substr(0, 12) == "server_name=")
			processServerName();
		else if (this->line.substr(0, 11) == "body_size =" || this->line.substr(0, 10) == "body_size=")
			processBodySize();
		else if (this->line.substr(0, 12) == "error_page =" || this->line.substr(0, 11) == "error_page=")
			processErrorPage();
		else if (this->line == "[location]" || this->line == "[server]")
			break ;
		else
		{
			std::cout << line << std::endl;
			throw std::runtime_error("Unknown directive!");
		}
	}
}


void ConfigParser::showConfigs()
{
	std::cout << "Number of servers: " << this->configs.servers.size() << std::endl;
	for (size_t i = 0; i < this->server_index; i++)
	{
		std::cout << std::endl << "START" << std::endl << std::endl;
		std::cout << "Host/Port:'" << this->configs.servers[i].host << "/" << this->configs.servers[i].port << "'" << std::endl;
		std::cout << "ServerNames: ";
		for (size_t j = 0; j < this->configs.servers[i].server_names.size(); j++)
			std::cout << "'" << this->configs.servers[i].server_names[j] << "', ";
		std::cout << std::endl;
		std::cout << "isVirtual:[" << this->configs.servers[i].isVirtual << "]\tisDefault:[" << this->configs.servers[i].isDefault << "]" << std::endl; 
		std::cout << "ErrorPages: ";
		std::map<int, std::string>::iterator it = this->configs.servers[i].error_pages.begin();
		for (; it != this->configs.servers[i].error_pages.end(); it++)
			std::cout << "[" << it->first << "]='" << it->second << "' ";
		std::cout << std::endl;
		std::cout << "Max_body_size: " << this->configs.servers[i].max_body_size << std::endl;
		for (size_t j = 0; j < this->configs.servers[i].locations.size(); j++)
		{
			std::cout << "path='" << this->configs.servers[i].locations[j].path << "'" << std::endl;
			std::cout << "root='" << this->configs.servers[i].locations[j].root << "'" << std::endl;
			std::cout << "methods: ";
			std::set<std::string>::iterator itm = this->configs.servers[i].locations[j].methods.begin();
			while (itm != this->configs.servers[i].locations[j].methods.end())
			{
				std::cout << *itm << " ";
				itm++;
			}
			std::cout << std::endl;
			std::cout << "redirects: ";
			std::map<int, std::string>::iterator it = this->configs.servers[i].locations[j].redirect.begin();
			while (it != this->configs.servers[i].locations[j].redirect.end())
			{
				std::cout << "[" << it->first << "]->[" << it->second << "]\t";
				it++;
			}
			std::cout << std::endl;
			std::cout << "directory_listing:" << this->configs.servers[i].locations[j].directory_listing << std::endl;
			std::cout << "default_file:'" << this->configs.servers[i].locations[j].default_file << "'" << std::endl;
			std::cout << "upload_store:'" << this->configs.servers[i].locations[j].upload_store << "'" << std::endl;
			std::cout << "CGI: ";
			std::map<std::string, std::string>::iterator itc = this->configs.servers[i].locations[j].cgi.begin();
			while (itc != this->configs.servers[i].locations[j].cgi.end())
			{
				std::cout << "\t[" << itc->first << "]='" << itc->second << "'" << std::endl;
				itc++;
			}
		}
		std::cout << std::endl << "END" << std::endl << std::endl;
	}
}