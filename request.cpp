#include <iostream>
#include <string>

#include "request.h"

#include <arpa/inet.h>  // for ip inet_pton()
#include <netinet/in.h> // for address
#include <sys/select.h> // for io multiplexing (select)
#include <sys/socket.h> // for socket
#include <unistd.h>  // for close()

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
    
    int sock = 0;
    struct sockaddr_in serv_addr;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
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
    recv(sock, &receiveBufferLength, sizeof(int), 0);

    char* receiveBuffer = (char*)calloc(1, receiveBufferLength);

    recv(sock, receiveBuffer, receiveBufferLength, 0);
    string response = receiveBuffer;
    free(receiveBuffer);

    // Close socket
    close(sock);
    return response;
}