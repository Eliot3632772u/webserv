#pragma once

#include "../server/Server.hpp"
#include "../config/ConfigFile.hpp"

struct RequestData{

    std::string method;
    std::string path;
    std::string uri;
    std::string host; // if no host header exists set to default
    std::string content_lenght;// if no content-header exists set fo false
    std::string transfer_encoding;// if no transfer_encoding exists set fo false
    std::string content_type;
    std::string connection;// not mentioned in the subject
    std::string cookie;
    std::vector<std::string> additionalHeaders;
    bool is_filled;
};

struct HttpRequest{

  // ConfigFile & serverConf;
  // Server & servers;
  int client_fd;
  std::string host;
  std::string port;
  bool readFirstTime;
  // std::map<int, std::string> client_request;
  // std::map<int, bool> client_first_read;
  RequestData headersData;

  // HttpRequest(ConfigFile & serverConf);

  HttpRequest();
  void readRequest();
  void handleRequest(std::string & client_req);
  void parseReadChunck(char * Buffer, ssize_t size);
  void parseHeaders(char * Buffer, ssize_t size);
  void isReqLineValid(std::string & requestLine, int client_fd);
//   void handleRequestLine(char * Buffer)
};

