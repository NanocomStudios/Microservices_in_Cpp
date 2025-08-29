#include <iostream>
#include <string>

#include "server.h"
#include "request.h"

using namespace std;

void serviceHandler(string msg,int sd){
    cout << msg << endl;
    response(msg, sd);
}

int main() {
    startServer("127.0.0.1",4277,serviceHandler,5);
    return 0;
}