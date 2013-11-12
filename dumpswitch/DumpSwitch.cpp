#include <stdio.h>
#include "DumpSwitch.h"
#include <string.h>
#include <iostream>
using namespace std;


DumpSwitch::DumpSwitch()
{
    //ctor

}


DumpSwitch::~DumpSwitch()
{
    //dtor

}

int DumpSwitch::init(){
        FILE* fp = fopen("../socket.txt","r");

        fscanf(fp,"%d",&m_port);
        fscanf(fp,"%d",&m_num_of_switches);
        m_switch_netid = new int[m_num_of_switches];
        for(int i=0;i<m_num_of_switches;i++){
            fscanf(fp,"%d",&m_switch_netid[i]);
        }

        fscanf(fp,"%d",&m_num_of_nodes);
        m_node_netid = new int[m_num_of_nodes];
        for(int i=0;i<m_num_of_nodes;i++){
            fscanf(fp,"%d",&m_node_netid[i]);
        }

        fclose(fp);

        m_connected = m_num_of_nodes;

        printf("port=%d num_of_switches = %d num_of_nodes=%d\n",m_port,m_num_of_switches,m_num_of_nodes);

        m_socket = new Socket*[m_num_of_nodes];

        m_server_socket.init(m_port);
        m_server_socket.registerEventListener(this);
}

int DumpSwitch::start(){
    m_server_socket.start();

}


int DumpSwitch::onAccept(Socket* socket){
    socket->registerEventListener(this);
    return 0;
}
int DumpSwitch::onDisconnect(ServerSocket* serverSocket){}

//SocketEventListener
int DumpSwitch::onConnect(Socket* socket){

}
int DumpSwitch::onReceive(char* message,Socket* socket){
    if(strlen(message) >=6){
        if(strncmp("NODE ",message,5)==0){
            int nodeid;
            sscanf(message+5,"%d",&nodeid);
            cout << "RECV NODE ID" << nodeid << endl;
            m_socket[nodeid] = socket;
            return 0;
        }
    }

    //regular msg

    unsigned int from=0, to=0;
    memcpy(&to,&message[0],1);
    memcpy(&from,&message[1],1);
    cout << "FORWARD MSG FROM " << from << " TO " << to << endl;

}
int DumpSwitch::onDisconnect(Socket* socket){}
