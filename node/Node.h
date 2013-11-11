#ifndef NODE_H
#define NODE_H
#include <set>
#include "NodeNetwork.h"

/*
Yu-Chun Lee yxl122130 11.1.2013
master class of algorithm nodes
*/

class Node
{
    public:
        Node();
        virtual ~Node();
        int virtual receive(int from, int to, int timestamp, char* message)=0;
        int send(int from, int to, int timestamp, char* message);
        int init();
    protected:
        const int m_node_id;
    private:
        NodeNetwork m_node_network;
};

#endif // NODE_H
