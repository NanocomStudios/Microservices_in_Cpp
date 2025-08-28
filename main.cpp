#include <iostream>
#include <string>

#include "server.h"

using namespace std;

void serviceHandler(string msg,int sd){
    cout << msg << endl;
    char x = msg[0];
    write(sd, &x, 1);
}

int main() {
    server(serviceHandler);
    return 0;
}