#ifndef NODENETWORK_H
#define NODENETWORK_H
#include "../socket/ServerSocket.h"
#include "../socket/Socket.h"
#include "../socket/ServerSocketEventListener.h"
#include "../socket/SocketEventListener.h"

/*
Yu-Chun Lee, yxl122130 11.8.2013
network layer of algorithm nodes

set m_mode=0 for algorithm tests and 1 for cooperation to switches
*/

class Node;

class NodeNetwork: public SocketEventListener
{
    public:
        NodeNetwork(Node* node,int node_id);
        int init();
        int start();

        int send(int from, int to, int timestamp, char* message);

        virtual ~NodeNetwork();



        //SocketEventListener
        int onConnect(Socket* socket);
        int onReceive(char* message,Socket* socket);
        int onDisconnect(Socket* socket);

        int close();

    protected:
    private:
        int getHostName(int netId,char* host);
        //Socket** m_sockets; //used when working without switch
        Socket* m_socket; //used when working with switch
        //ServerSocket m_server_socket;
        //int m_node_number;
        int m_num_of_nodes;
        //int* m_node_netid;
        //int* m_switch_netid;

        int m_switch_netid;
        int m_num_of_switches;

        int m_node_id;

        Node* m_node;

        int m_port;
        //int m_mode;
};

#endif // NODENETWORK_H
