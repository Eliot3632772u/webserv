#include "httpRequest.hpp"
#include "../server/Server.hpp"

HttpRequest::HttpRequest() : readFirstTime(false){}

// void HttpRequest::handleRequest(std::string & client_req){

//     /* # check if the req form is valid
//             -- is reques_line valid 
//             -- is method valid return 405 method not valid
//             -- is HTTP version valid
//             -- is URI lenght is <= 2000 
//             -- if Request uri contain a character not allowded
//             -- does request line and method lines end with CRLF
//             -- if Transfer-Encoding header exist and is different to “chunked
//             -- if Transfer-Encoding not exist Content-Length not exist The method is Post
//             -- if Request body larger than client max body size in config fil
//             -- check for duplicate headers

//        # handle each method sepratelly 
//             -- decode and check path if it exists 

//        */
// }

// bool is_uri_valid(std::string & uri){


// }
// bool is_method_valid(std::string & method){

    
// }

// void HttpRequest::isReqLineValid(std::string & requestLine, int client_fd){

//     std::stringstream reqStream(requestLine);
//     std::string method;
//     std::string httpversion;
//     std::string uri;
//     std::string leftovers;

//     std::getline(reqStream, method, ' ');
//     std::getline(reqStream, uri, ' ');
//     std::getline(reqStream, httpversion, '\r');
//     std::getline(reqStream, leftovers);

//     if (requestLine.size() < 18 || requestLine.size() > 2019 || requestLine[requestLine.size() - 1] != '\r'){

//             // handle response here , remove clien_fd from map and anything related to it
//             throw -1;
//     }

//     if (leftovers != "\r"){

//         // handle response here , remove clien_fd from map and anything related to it
//         throw -1;

//     }else if (!is_uri_valid(uri)){

//         // handle response here , remove clien_fd from map and anything related to it
//         throw -1;

//     }else if (!is_method_valid(method)){

//         // handle response here , remove clien_fd from map and anything related to it
//         throw -1;

//     }else if (httpversion != "HTTP/1.1"){

//         // handle response here , remove clien_fd from map and anything related to it
//         throw -1;
//     }

//     RequestData & client_data = client_req_data[client_fd];

//     client_data.method = method;
//     client_data.uri = uri;
// }

void HttpRequest::parseReadChunck(char * Buffer, ssize_t size){

    // ssize_t pos = 0;
    // std::stringstream BufferStream(Buffer);
    // std::string requestLine;

    // if (size < 18){

    //     // handle response here , remove clien_fd from map and anything related to it
    //         throw -1;
    // }
    // if (client_first_read[client_fd] == false){

    //     std::getline(BufferStream, requestLine);
        
    //     try{

    //         isReqLineValid(requestLine, client_fd);
    //     }catch(...){

    //         throw -1;
    //     }
    //     client_first_read[client_fd] = true;
    // }
    // else{
        
        
    // }

    // if (this->readFirstTime == false)
    //     isReqLineValid()
    
}

bool isUriValid(std::string uri){

    
}

std::string trim(const std::string& str) {

    size_t start = 0;
    while (start < str.length() && (str[start] == ' ' || str[start] == '\t'))
        ++start;

    size_t end = str.length();
    while (end > start && (str[end - 1] == ' ' || str[end - 1] == '\t'))
        --end;

    return str.substr(start, end - start);
}

void HttpRequest::readRequest(){

    char buffer[8125];

    memset(buffer, 0, 8125);

    ssize_t read_res = read(this->client_fd, buffer, 8124);
    if (read_res == -1)
        return ; // should return a number to check in server part

    if (readFirstTime == false){

        std::string bufferString = buffer;
        if (read_res < 18){
            
            // throw status error;
        }
        if (bufferString.find("\r\n\r\n") == std::string::npos){
           
            // throw status error;
        }
        std::stringstream bufferStream(bufferString);
        std::string reqLine;
        std::getline(bufferStream, reqLine);

        if (reqLine.empty() || reqLine[reqLine.size() - 1] != '\r'){
            // throw status error;
        }

        std::stringstream reqLineStream(reqLine);
        std::string method;
        std::string uri;
        std::string http;

        reqLineStream >> method >> uri >> http;

        if (reqLineStream.fail() || !reqLineStream.eof()){
            // throw status error;
        }

        if (http != "HTTP/1.1"){
            // throw status error;
        }

        if (!isUriValid(uri)){
            // throw status error;
        }

        if (method != "GET" && method != "DELETE" && method != "POST"){
            // throw status error;
        }

        this->headersData.method = method;
        this->headersData.uri = uri;

        while (bufferStream.eof()){

            std::string headerLine;
            std::getline(bufferStream, headerLine);
            

            if (headerLine.empty() || headerLine[headerLine.size() - 1] != '\r'){
                // throw status code;
            }

            std::stringstream headerStream(headerLine);
            std::string key;
            std::string value;

            std::getline(headerStream, key, ':');
            std::getline(headerStream, value);
            
            if (key.empty() || value.empty()){
                // throw
            }

            value = trim(value);

            if (key == "Content-Type"){

                if (!this->headersData.content_type.empty()){
                    // throw status code
                }
            }
            else if (key == "Host"){

            }
            else if (key == "Transfer-Encoding"){

            }else if (key == "Content-Lenght"){

            }else if (key == "Connection"){

            }else if (key == "Cookie"){

            }


        }




        this->readFirstTime = true;
    }
    // try{

    //     parseReadChunck(Buffer, read_res);
    
    // }catch(...){

    //     throw -1;
    // }
    // // int client_req_fd = 
    // char Buffer[1024];

    // memset(Buffer, 0, 1024);
    // // read request in to buffer than join it to the client_req;
    // // check return value of read , if 0 or the size of readbytes is less than the buffer the req is done, handle req than remove client_fd from map
    // // if return value is -1 , return to handle other requests

    // ssize_t read_res = read(client_fd, Buffer, 1024);
    // if (read_res == -1)
    //     return ; // should return a number to check in server part

    // // after read parse the data then 
    // client_req.append(Buffer, read_res);

    // if (read_res < 1024){

    //     handleRequest(client_req);
    //     client_request.erase(client_fd);
    //     close(client_fd);
    //     return;
    // }
}


		// mimeTypes.insert(std::make_pair(".html", "text/html"));
		// mimeTypes.insert(std::make_pair(".htm", "text/html"));
		// mimeTypes.insert(std::make_pair(".css", "text/css"));
		// mimeTypes.insert(std::make_pair(".csv", "text/csv"));
		// mimeTypes.insert(std::make_pair(".doc", "application/doc"));
		// mimeTypes.insert(std::make_pair(".gif", "image/gif"));
		// mimeTypes.insert(std::make_pair(".jpg", "image/jpeg"));
		// mimeTypes.insert(std::make_pair(".jpeg", "image/jpeg"));
		// mimeTypes.insert(std::make_pair(".js", "text/javascript"));
		// mimeTypes.insert(std::make_pair(".json", "application/json"));
		// mimeTypes.insert(std::make_pair(".jar", "application/java-archive"));
		// mimeTypes.insert(std::make_pair(".mp3", "audio/mpeg"));
		// mimeTypes.insert(std::make_pair(".mp4", "video/mp4"));
		// mimeTypes.insert(std::make_pair(".mpeg", "video/mpeg"));
		// mimeTypes.insert(std::make_pair(".png", "image/png"));
		// mimeTypes.insert(std::make_pair(".pdf", "application/pdf"));
		// mimeTypes.insert(std::make_pair(".sh", "application/x-sh"));
		// mimeTypes.insert(std::make_pair(".wav", "audio/wav"));
		// mimeTypes.insert(std::make_pair(".weba", "audio/webm"));
		// mimeTypes.insert(std::make_pair(".webm", "video/webm"));
		// mimeTypes.insert(std::make_pair(".webp", "image/webp"));
		// mimeTypes.insert(std::make_pair(".xml", "application/xml"));
		// mimeTypes.insert(std::make_pair(".zip", "application/zip"));
		// mimeTypes.insert(std::make_pair(".tar", "application/x-tar"));
		// mimeTypes.insert(std::make_pair(".bin", "application/octet-stream"));
		// mimeTypes.insert(std::make_pair(".avif", "image/avif"));
		// mimeTypes.insert(std::make_pair(".avi", "video/x-msvideo"));