#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include "DumpSwitch.h"

using namespace std;

int main(int argc, char*argv [])
{
    /*NodeNetwork nodeNetwork(0);
    nodeNetwork.start();

    char c;
    cin >> c;
    nodeNetwork.close();*/
    DumpSwitch ds;
    ds.init();
    ds.start();


    while(true){
        usleep(100);
        if(ds.end()){
            break;
        }
    }
    ds.close();
    return 0;
}
