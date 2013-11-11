#include <iostream>
#include <stdlib.h>
#include "./node/LAKNode.h"

using namespace std;

int main(int argc, char*argv [])
{
    /*NodeNetwork nodeNetwork(0);
    nodeNetwork.start();

    char c;
    cin >> c;
    nodeNetwork.close();*/
    if(argc < 3){
        cout << "argument: [node_id] [algorithm]" << endl;
        cout << "[algorithm]: 0=LAC 1=Maekawa" << endl;
        return 0;
    }
    int nodeid = atoi(argv[1]);
    int algorithm = atoi(argv[2]);

    if(algorithm==0){
        LAKNode node(nodeid);
        node.init();
        node.start();
    }else{

    }
    return 0;
}
