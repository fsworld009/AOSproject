#ifndef LISTENSERVER_H
#define LISTENSERVER_H
#include "../socket/ServerSocketEventListener.h"
#include "../socket/ServerSocket.h"
#include "../socket/SocketEventListener.h"
#include "../socket/Socket.h"
#include <vector>
using namespace std;

class ListenServer: public ServerSocketEventListener, SocketEventListener
{
    public:
        ListenServer();
        virtual ~ListenServer();
        int onAccept(Socket* socket);
        int onReceive(char* message,Socket* socket);
        int onDisconnect(Socket* socket);
        int onDisconnect(ServerSocket* serversocket);
        int init();
        int start();
        int close();
        bool end();
        bool m_end;
    protected:
    private:
        vector<Socket*> m_accept_socket;
        ServerSocket m_server_socket;
        int m_port;
};

#endif // ECHOSERVER_H
