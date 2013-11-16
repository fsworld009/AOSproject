#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include "LAKNode.h"
#include "MAKNode.h"

using namespace std;

int main(int argc, char*argv [])
{

    if(argc < 3){
        cout << "argument: [node_id] [algorithm]" << endl;
        cout << "[algorithm]: 0=LAK 1=Maekawa" << endl;
        return 0;
    }
    int nodeid = atoi(argv[1]);
    int algorithm = atoi(argv[2]);



    if(algorithm==0){
        LAKNode node(nodeid);
        node.init();
        node.start();
        //node.close();
    }else{
        MAKNode node(nodeid);
        node.init();
        node.start();
    }

    return 0;
}
