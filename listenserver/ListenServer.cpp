#include "ListenServer.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

ListenServer::ListenServer()
{
    m_end=false;
    //ctor
    m_port=0;
    //m_node_thread=0;
    m_accept_socket=0;
}

bool ListenServer::end(){

    return m_end;

}

int ListenServer::init(){
        FILE* fp = fopen("./config/lserver.txt","r");

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
    //m_accept_socket.push_back(socket);
    //if(m_accept_socket==0){
        m_accept_socket = socket;
    //}else{
        //di
    //}

    socket->registerEventListener(this);
    return 0;
}

int ListenServer::onReceive(char* message,Socket* socket){
    //handle incoming message here
    /*
    case get switch id:
    fork vswitch?
    
    
    case get node id:
    if(m_node_thread ==0){
        m_node_thread = new NodeThread(node_id);
    }
    case get set_LAK algorithm signal:
    m_node_thread->set_algorithm(0);
    m_node_thread->start();
    
    case get set_Maekawa algorithm signal:
    m_node_thread->set_algorithm(1);
    m_node_thread->start();
     
    m_node_thread = new Maekawa(node_id);
    

    */
    //test
    if(strncmp(message,"DSWITCH",6)==0){
        system("./dumpswitch.out");
    }else if(strncmp(message,"NODE ",5)==0){
        int nodeid=0,algorithm=0;
        sscanf(message+5,"%d %d",&nodeid,&algorithm);
        char buff[20];
        sprintf(buff,"./node.out %d %d",nodeid,algorithm);
        system(buff);
    }
    
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

    /*for(unsigned int i=0;i<m_accept_socket.size();i++){
        m_accept_socket[i]->disconnect();
        delete m_accept_socket[i];
    }
    m_accept_socket.clear();*/
    if(m_accept_socket != 0){
        m_accept_socket->disconnect();
        delete m_accept_socket;
        //m_accept_socket.close();
    }

    return 0;

}

ListenServer::~ListenServer()
{
    //dtor
    close();
}
/*
ListenServer::NodeThread::NodeThread(int node_id){
    m_node_id = node_id;
    m_node=0;
}

int ListenServer::NodeThread::set_algorithm(int algorithm){
    if(m_node != 0){
        delete m_node;
    }
    switch(algorithm){
        case 0:
            m_node=new LAKNode(m_node_id);
            break;
        case 1:
            //m_node=new MaekawaNode(m_node_id);
            break;
        
    }
    

}

int ListenServer::NodeThread::run(){
    m_node->init();
    m_node->start();
    
}*/