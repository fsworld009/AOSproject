#ifndef SERVERSOCKETEVENTLISTENER_H
#define SERVERSOCKETEVENTLISTENER_H
#include "Socket.h"
#include "ServerSocket.h"

/*
Yu-Chun Lee yxl122130
10.26.2013
a class that listens to events from ServerSocket such as accept a socket
extends this class to become a listener
*/

class ServerSocketEventListener
{
    public:
        ServerSocketEventListener();
        virtual ~ServerSocketEventListener();
        int virtual onAccept(Socket* socket);
        int virtual onDisconnect(ServerSocket* serverSocket);
    protected:
    private:

};

#endif // SERVERSOCKETEVENTLISTENER_H
