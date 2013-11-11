#include <iostream>
#include "./node/NodeNetwork.h"

using namespace std;

int main()
{
    NodeNetwork nodeNetwork(0);
    nodeNetwork.start();

    char c;
    cin >> c;
    nodeNetwork.close();
    return 0;
}
