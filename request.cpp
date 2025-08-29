#if defined(_WIN32) || defined(_WIN64)

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

#define SOCKET_TYPE SOCKET

WSADATA SwsaData;
int SiResult;

#elif defined(__linux__)

#include <arpa/inet.h>  // for ip inet_pton()
#include <netinet/in.h> // for address
#include <sys/select.h> // for io multiplexing (select)
#include <sys/socket.h> // for socket
#include <unistd.h>  // for close()

#define SOCKET_TYPE int
#endif

#include <iostream>
#include <string>
#include <stdlib.h>

#include "request.h"

using namespace std;

string request(string message, string ipAddress, int port) {

    int messageLength = message.length() + 1; // +1 for null terminator

    char* sendBuffer = (char*)calloc(1, messageLength + 4);
    *(int*)sendBuffer = messageLength;

    int i = 0;
    for(char x : message){
        sendBuffer[4 + i] = x;
        i++;
    }

    message.erase();
    
#if defined(_WIN32) || defined(_WIN64)
    SiResult = WSAStartup(MAKEWORD(2, 2), &SwsaData);
    if (SiResult != 0) {
        printf("WSAStartup failed with error: %d\n", SiResult);
        return "";
    }

    SOCKET sock = 0;
#elif defined(__linux__)
    int sock = 0;
#endif

    struct sockaddr_in serv_addr;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    #if defined(_WIN32) || defined(_WIN64)
        if (sock == INVALID_SOCKET) {
            std::cerr << "Socket creation error" << std::endl;
            return "";
        }
    #elif defined(__linux__)
        if (sock <= 0) {
            std::cerr << "Socket creation error" << std::endl;
            return "";
        }
    #endif

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary
    if (inet_pton(AF_INET, ipAddress.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return "";
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return "";
    }

    // Send data
    send(sock, sendBuffer, messageLength + 4, 0);

    free(sendBuffer);
    std::cout << "Message sent" << std::endl;

    int receiveBufferLength;
    recv(sock, (char*)&receiveBufferLength, (int)sizeof(int), 0);

    char* receiveBuffer = (char*)calloc(1, receiveBufferLength);

    recv(sock, receiveBuffer, receiveBufferLength, 0);
    string response = receiveBuffer;
    free(receiveBuffer);

    // Close socket
    #if defined(_WIN32) || defined(_WIN64)
        closesocket(sock);
        WSACleanup();
    #elif defined(__linux__)
        close(sock);
    #endif
    return response;
}