#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0600

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <iostream>
#include <thread>

#include <vector>

#include "server.h"
using namespace std;

#pragma comment (lib, "Ws2_32.lib")

struct clientDetails{
      SOCKET clientfd;  // client file descriptor
      SOCKET serverfd;  // server file descriptor
      std::vector<SOCKET> clientList; // for storing all the client fd
      clientDetails(void){ // initializing the variable
            this->clientfd=-1;
            this->serverfd=-1;
      }
};

WSADATA SwsaData;
int SiResult;

void startServer(string ip, int port, void(*serviceHandler)(std::string,SOCKET),int backlog){
    // Initialize Winsock
    SiResult = WSAStartup(MAKEWORD(2, 2), &SwsaData);
    if (SiResult != 0) {
        printf("WSAStartup failed with error: %d\n", SiResult);
        return;
    }

    // creating a clientDetails object{
    auto client= new clientDetails();

    client->serverfd= socket(AF_INET, SOCK_STREAM,0); // for tcp connection
    // error handling
    if (client->serverfd==INVALID_SOCKET){
        std::cerr<<"socket creation error\n";
        WSACleanup();
        delete client;
        exit(1);
    }else{
        std::cout<<"socket created\n";
    }

    // setting serverFd to allow multiple connection
    int opt=1;
    if (setsockopt(client->serverfd,SOL_SOCKET,SO_REUSEADDR, (char*)&opt, sizeof opt)==SOCKET_ERROR){
        std::cerr<<"setSocketopt error\n";
        closesocket(client->serverfd);
        WSACleanup();
        delete client;
        exit(2);
    }
    else{
        std::cout<<"setSocketopt success\n";
    }

    // setting the server address
    struct sockaddr_in serverAddr;
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons(port);
    
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

    // binding the server address
    if (bind(client->serverfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr))==SOCKET_ERROR){
        std::cerr<<"bind error\n";
        closesocket(client->serverfd);
        WSACleanup();
        delete client;
        exit(3);
    }else{
        std::cout<<"server binded\n";
    }

    // listening to the port
    if (listen(client->serverfd, backlog)==SOCKET_ERROR){
        std::cerr<<"listen error\n";
        closesocket(client->serverfd);
        WSACleanup();
        delete client;
        exit(4);
    }else{
        std::cout<<"server is listening\n";
    }

    fd_set readfds;
    size_t  valread;

    int maxfd;
    int sd=0;
    int activity;
    while (true){
        std::cout<<"waiting for activity\n";
        FD_ZERO(&readfds);
        FD_SET(client->serverfd, &readfds);
        maxfd=client->serverfd;
        // copying the client list to readfds
        // so that we can listen to all the client
        for(auto sd:client->clientList){
            FD_SET(sd, &readfds);
            if (sd>maxfd){
                maxfd=sd;
            }
        }
        /* using select for listen to multiple client
           select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds,
           fd_set *restrict errorfds, struct timeval *restrict timeout);
        */

        // for more information about select
        activity=select(maxfd+1, &readfds, NULL, NULL, NULL);
        if (activity<0){
            std::cerr<<"select error\n";
            continue;
        }
        /*
         * new connection request
         */

        if (FD_ISSET(client->serverfd, &readfds)) {
            client->clientfd = accept(client->serverfd, (struct sockaddr *) NULL, NULL);
            if (client->clientfd == INVALID_SOCKET) {
                std::cerr << "accept error\n";
                continue;
            }
            // adding client to list
            client->clientList.push_back(client->clientfd);
            std::cout << "new client connected\n";
            /*
             * std::thread t1(handleConnection, client);
             * t1.detach();
             *handle the new connection in new thread
             */
        }

        // checking all the client for incoming message
        for(int i=0;i<client->clientList.size();++i){
            sd=client->clientList[i];
            if (FD_ISSET(sd, &readfds)){
                char buffer[1024]={0};
                // read the message length first
                int messageLength;
                valread=recv(sd, (char*)&messageLength, 4,0);
                if (valread==0){
                    // some error or client disconnected
                    std::cout<<"client disconnected\n";
                    closesocket(sd);
                    // removing from the list
                    client->clientList.erase(client->clientList.begin()+i);
                    continue;
                }
                // read the message
                valread=recv(sd, buffer, messageLength,0);
                std::string message=buffer;

                std::cout<<"message from client: "<<message << " in " << messageLength<<"\n";
                // write(sd, "hello", 5); // echoing back the message
                std::thread t1(serviceHandler, message, sd);
                //detach the thread so that it can run independently
                t1.detach();
                message.erase();

            }
        }
    }
    // closing the server socket
    closesocket(client->serverfd);
    WSACleanup();
    delete client;
}

void response(string message,SOCKET_TYPE dt){
    int messageLength = message.length() + 1; // +1 for null terminator

      char* sendBuffer = (char*)calloc(1, messageLength + 4);
      *(int*)sendBuffer = messageLength;

      int i = 0;
      for(char x : message){
         sendBuffer[4 + i] = x;
         i++;
      }

      send(dt, sendBuffer, messageLength + 4,0);
      free(sendBuffer);
}