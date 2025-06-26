#pragma once 

#include <iostream>
#include <vector>
#include <map>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <sys/epoll.h>
#include "../config/ServerConfig.hpp"
#include "../config/LocationConfig.hpp"
#include "../config/ConfigFile.hpp"
#include "../server/Server.hpp"
