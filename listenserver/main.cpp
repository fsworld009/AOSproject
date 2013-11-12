#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include "ListenServer.h"

using namespace std;

int main(int argc, char*argv [])
{
    /*NodeNetwork nodeNetwork(0);
    nodeNetwork.start();

    char c;
    cin >> c;
    nodeNetwork.close();*/
    ListenServer server;
    server.init();
    server.start();


    while(true){
        usleep(100);
        if(server.end()){
            break;
        }
    }
    server.close();
    return 0;
}
