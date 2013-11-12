#include "ListenServer.h"
#include <stdio.h>
#include <iostream>
using namespace std;

ListenServer::ListenServer()
{
    m_end=false;
    //ctor
    m_port=0;
}

bool ListenServer::end(){

    return m_end;

}

int ListenServer::init(){
        FILE* fp = fopen("../socket.txt","r");

        fscanf(fp,"%d",&m_port);
        fclose(fp);
        
}

int ListenServer::start(){
    m_server_socket.init(m_port);
    m_server_socket.registerEventListener(this);
    cout << "ListenServer Start" << endl;
    m_server_socket.start();
    return 0;
}

int ListenServer::onAccept(Socket* socket){
    m_accept_socket.push_back(socket);

    socket->registerEventListener(this);
    return 0;
}

int ListenServer::onReceive(char* message,Socket* socket){
    //handle incoming message here
    return 0;
}

int ListenServer::onDisconnect(Socket* socket){
    //m_accept_socket.removeElement(socket)
    //delete socket;
    return 0;
}

int ListenServer::onDisconnect(ServerSocket* serversocket){
    return 0;
}

int ListenServer::close(){
    m_server_socket.disconnect();

    for(unsigned int i=0;i<m_accept_socket.size();i++){
        m_accept_socket[i]->disconnect();
        delete m_accept_socket[i];
    }
    m_accept_socket.clear();

    return 0;

}

ListenServer::~ListenServer()
{
    //dtor
    close();
}
