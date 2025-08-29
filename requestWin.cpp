#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0600

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <iostream>
#include <thread>

#include <vector>

#include "request.h"

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

WSADATA SwsaData;
int SiResult;

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

    SiResult = WSAStartup(MAKEWORD(2, 2), &SwsaData);
    if (SiResult != 0) {
        printf("WSAStartup failed with error: %d\n", SiResult);
        return "";
    }

    SOCKET sock = 0;
    struct sockaddr_in serv_addr;
    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation error" << std::endl;
        return "";
    }

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
    recv(sock, (char*)&receiveBufferLength, 4, 0);

    char* receiveBuffer = (char*)calloc(1, receiveBufferLength);

    recv(sock, receiveBuffer, receiveBufferLength, 0);
    string response = receiveBuffer;
    free(receiveBuffer);

    // Close socket
    closesocket(sock);
    return response;
}
