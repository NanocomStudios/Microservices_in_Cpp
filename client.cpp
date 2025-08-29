#include <iostream>
#include <string>
using namespace std;

#include "request.h"

int main(){
    string response = request("Request from the client module","127.0.0.1",4277);
        
    if(!response.empty()){
        cout << response << endl;
    }
    
}