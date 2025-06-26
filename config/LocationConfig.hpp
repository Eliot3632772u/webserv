#pragma once

#include "../inc/webserv.hpp"

struct LocationConfig {

    std::string path;              // e.g., "/upload"
    std::string root;              // e.g., "/var/www/html"
    std::vector<std::string> methods; // GET, POST
    bool autoindex = false;
    std::string index_file;
    std::string redirect_url;      // if set → redirect
    std::string upload_store_path;
    std::string cgi_extension;     // e.g., ".php"
    std::string cgi_pass;          // e.g., "/usr/bin/php-cgi"
};