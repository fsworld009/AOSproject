#ifndef NODE_H
#define NODE_H
#include <set>
#include <string>
#include "NodeNetwork.h"
using namespace std;

/*
Yu-Chun Lee yxl122130 11.1.2013
master class of algorithm nodes
*/

class Node
{
    public:
        Node(int node_id);
        virtual ~Node();
        int virtual receive(int from, int to, int timestamp, string message)=0;

        int send(int from, int to, int timestamp, string message);
        int init();
        int start();
    protected:
        const int node_id;
        set<int> quorum_set;
        set<int> time_schedule;
    private:
        NodeNetwork m_node_network;
        int parse_quorum();
        int parse_schedule();
        int virtual run()=0;
};

#endif // NODE_H
