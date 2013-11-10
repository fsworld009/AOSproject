#ifndef NODENETWORK_H
#define NODENETWORK_H
#include "ServerSocket.h"
#include "Socket.h"
#include "ServerSocketEventListener.h"
#include "SocketEventListener.h"

class Node;

class NodeNetwork: public ServerSocketEventListener, SocketEventListener
{
    public:
        NodeNetwork(Node* node);
        int start();

        int send(int from, int to, int timestamp, char* message);

        virtual ~NodeNetwork();

        //ServerSocketEventListener
        int onAccept(Socket* socket);
        int onDisconnect(ServerSocket* serverSocket);

        //SocketEventListener
        int onConnect(Socket* socket);
        int onReceive(char* message,Socket* socket);
        int onDisconnect(Socket* socket);

    protected:
    private:
        int init();
        Socket** m_sockets; //used when working without switch
        Socket* m_socket; //used when working with switch
        ServerSocket m_server_socket;
        //int m_node_number;
        int m_num_of_nodes;
        int* m_netid;
        Node* m_node;

        int m_port;
        int m_mode;
};

#endif // NODENETWORK_H
