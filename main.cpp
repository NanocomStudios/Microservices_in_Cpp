#include <iostream>
#include <string>

#include "server.h"
#include "client.h"

using namespace std;

void serviceHandler(string msg,int sd){
    cout << msg << endl;
    response(msg, sd);
}

int main() {
    server(serviceHandler);
    return 0;
}