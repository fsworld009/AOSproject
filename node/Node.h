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

enum MessageType
    {
        REQUEST,
        RELEASE,
        RECEIVE,
        ACK,
        RELAY,
        TOKEN
    };

class Node
{
    public:
        Node(int node_id);
        virtual ~Node();
        //int virtual receive(int from, int to, int timestamp, string message)=0;
        int virtual receive_message(string message)=0;

        int send(unsigned int from, unsigned int to, unsigned int timestamp, string message);
        int init();
        int start();

        int signal();
    protected:
        const int node_id;
        set<int> quorum_set;
        set<int> time_schedule;
    private:
        NodeNetwork m_node_network;
        MutexLock m_wait_lock;
        int waitForSignal();
        bool m_wait;
        int parse_quorum();
        int parse_schedule();
        int virtual run()=0;
};

#endif // NODE_H
