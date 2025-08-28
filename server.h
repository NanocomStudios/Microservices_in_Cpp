#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>  // for ip inet_pton()
#include <netinet/in.h> // for address
#include <sys/select.h> // for io multiplexing (select)
#include <sys/socket.h> // for socket
#include <unistd.h>  // for close()

#include <string>
using std::string;

void server(void(*serviceHandler)(string,int));


#endif