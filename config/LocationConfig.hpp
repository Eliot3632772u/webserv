#pragma once

#include "../inc/webserv.hpp"

// should be inited to default in constructor 
// some should be given default values in constructor
class LocationConfig
{
	public:
		std::string path;
		std::string root;
		std::set<std::string> methods;
		std::map<int, std::string> redirect;
		bool directory_listing; 
		std::string default_file;
		std::string upload_store;
		std::map<std::string, std::string> cgi;
};
