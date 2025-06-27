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




// std::cout << "Waiting..." << std::endl;
//         int ready_fds = epoll_wait(this->epollFd, events, 100, -1);
//         if (ready_fds < 0){
            
//             close(this->epollFd);
//             initSocketsError("epoll_event: ", this->SocketFds, NULL);
//         }
//         std::cout << ready_fds << std::endl;


//         for (int i = 0; i < ready_fds; i++){

//             int sock_fd = events[i].data.fd;
//             if (std::find(this->SocketFds.begin(), this->SocketFds.end(), sock_fd) != this->SocketFds.end()){

//                 int client_fd = accept(sock_fd, NULL, NULL);
//                 if (client_fd < 0){

//                     close(this->epollFd);
//                     initSocketsError("accept: ", this->SocketFds, NULL);
//                 }

//                 fcntl(client_fd, F_SETFL, O_NONBLOCK);

//                 char BUF[100];
//                 if (read(client_fd, BUF, 100) == -1){

//                     struct epoll_event event;
//                     event.events = EPOLLIN | EPOLLOUT | EPOLLET;
//                     event.data.fd = client_fd;

//                     if (epoll_ctl(this->epollFd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
                    
//                         close(this->epollFd);
//                         close(client_fd);
//                         initSocketsError(NULL, client_fds, NULL);
//                         initSocketsError("epoll_ctl", this->SocketFds, NULL);
//                     }

//                     client_fds.push_back(client_fd);
//                     std::cout << "hello hanabab" << std::endl;
//                     continue;
//                 }
//                 // parse request
//                 // send response

//                 close(client_fd);
//             }
//             else if(std::find(client_fds.begin(), client_fds.end(), sock_fd) != client_fds.end()) {

//                 char BUF[100];
//                 if (read(sock_fd, BUF, 100) == -1){

//                     perror("read: ");
//                     exit(1);
//                 }

//                 std::cout << BUF << std::endl;
//                 close(sock_fd);
//                 if (epoll_ctl(this->epollFd, EPOLL_CTL_DEL, sock_fd, NULL) < 0){
//                     perror("epoll_ctl: ");
//                     exit(1);
//                 }
//                 client_fds.erase(std::find(client_fds.begin(), client_fds.end(), sock_fd));
//             }
//             else{
//                 std::cout << "no such socket linked to this fd: " << sock_fd << std::endl;
//             }
//         }