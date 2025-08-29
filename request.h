#ifndef REQUEST_H
#define REQUEST_H

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    #define SOCKET_TYPE SOCKET
#elif defined(__linux__)
    #define SOCKET_TYPE int
#endif

#include <string>
using std::string;

string request(string message, string ipAddress, int port);

#endif