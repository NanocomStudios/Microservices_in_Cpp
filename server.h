#ifndef SERVER_H
#define SERVER_H

#include <string>
using std::string;

void startServer(string ip, int port, void(*serviceHandler)(std::string,int),int backlog);
void response(string message,int dt);

#endif