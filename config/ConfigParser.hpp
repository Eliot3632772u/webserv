#pragma once

#include "../inc/webserv.hpp"
#include "ConfigFile.hpp"


class ConfigParser
{
	private:
		size_t server_index;
		ConfigFile configs;
		std::ifstream ifs;
		std::string line;
		void parseServerBlock();
		void processHost();
		void processPort();
		void processServerName();
		void processBodySize();
		void processErrorPage();
		
		void parseLocationBlock();
		void processPath();
		void processRoot();
		void processMethods();
		void processRedirect();
		void processDirectoryListing();
		void processDefaultFile();
		void processUpload();
		void processCGI();
	public:
		ConfigParser();
		ConfigFile parseConfigFile(const char* fileName);
		void showConfigs();
		void validateConfigs();
		ConfigFile& getConfigs();
};