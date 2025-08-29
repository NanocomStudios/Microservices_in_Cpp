#ifndef SERVER_H
#define SERVER_H

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    #define SOCKET_TYPE SOCKET
#elif defined(__linux__)
    #define SOCKET_TYPE int
#endif

#include <string>
using std::string;

void startServer(string ip, int port, void(*serviceHandler)(std::string,SOCKET_TYPE),int backlog);
void response(string message,SOCKET_TYPE dt);

#endif