#include <stdio.h>
#include <unistd.h>
#include "DumpSwitch.h"
#include <string.h>
#include <iostream>
using namespace std;


DumpSwitch::DumpSwitch()
{
    //ctor
    m_end=false;
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
        m_disconnected=0;

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
            cout << "RECV NODE ID " << nodeid << endl;
            m_socket[nodeid-1] = socket;

            m_connected--;
            if(m_connected==0){
                cout << "SEND START SIGNAL" << endl;
                for(int i=0;i<m_num_of_nodes;i++){
                    m_socket[i]->send("START");
                }
            }


            return 0;
        }
    }

    //regular msg

    unsigned int from=0, to=0;
    char buff[SOCKET_MAX_BUFFER_SIZE];
    memcpy(&to,&message[0],1);
    memcpy(&from,&message[1],1);
    bzero(buff,SOCKET_MAX_BUFFER_SIZE);
    memcpy(buff,&message[6],SOCKET_MAX_BUFFER_SIZE-6);

    //forward msg
    if(to>=1 && to <=m_num_of_nodes){
        cout << "FORWARD MSG " << buff << " FROM " << from << " TO " << to << endl;
        this->m_socket[to-1]->send(message);
    }else{
        cout << "Discard illegal message, please check algorithm design" << endl;
    }


}
int DumpSwitch::onDisconnect(Socket* socket){
    for(int i=0;i<m_num_of_nodes;i++){
        if(socket==m_socket[i]){

            m_disconnected++;
            delete socket;
            m_socket[i]=0;
            if(m_disconnected==m_num_of_nodes){
                //cout << "all disconnected" << endl;
                m_end=true;
            }
        }
    }
    return 0;

}

bool DumpSwitch::end(){

    return m_end;

}

int DumpSwitch::close(){
    m_server_socket.disconnect();
}
