#include "ServerSocketEventListener.h"


ServerSocketEventListener::ServerSocketEventListener()
{
    //ctor
}

int ServerSocketEventListener::onAccept(Socket* socket){
    return 0;
}

int ServerSocketEventListener::onDisconnect(ServerSocket* serversocket){
    return 0;
}


ServerSocketEventListener::~ServerSocketEventListener()
{
    //dtor
}

