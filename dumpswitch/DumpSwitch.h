#ifndef DUMPSWITCH_H
#define DUMPSWITCH_H
#include "../socket/ServerSocket.h"
#include "../socket/Socket.h"
#include "../socket/ServerSocketEventListener.h"
#include "../socket/SocketEventListener.h"
#include <fstream>

/*
Yu-Chun Lee yxl122130
11.11.2013
dump switch, simply forward incoming messages
*/

class DumpSwitch: public ServerSocketEventListener, SocketEventListener
{
    public:
        DumpSwitch();
        virtual ~DumpSwitch();
        int init();
        int start();
        int close();
        bool end();
    protected:
    private:
        ServerSocket m_server_socket;
        int m_port;
        int m_num_of_nodes;
        int m_num_of_switches;
        int m_connected;
        int m_completed;
        int m_disconnected;
        int* m_node_netid;
        int* m_switch_netid;

        bool m_end;
        //MutexLock m_end_lock;
        Socket** m_socket;

        //ServerSocketEventListener
        int onAccept(Socket* socket);
        int onDisconnect(ServerSocket* serverSocket);

        //SocketEventListener
        int onConnect(Socket* socket);
        int onReceive(char* message,Socket* socket);
        int onDisconnect(Socket* socket);
        std::ofstream m_logfile;
};

#endif // DUMPSWITCH_H
