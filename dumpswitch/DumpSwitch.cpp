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

        m_logfile.open("log_dumpserver.txt");

        m_connected = 0;
        m_disconnected=0;

        printf("port=%d num_of_switches = %d num_of_nodes=%d\n",m_port,m_num_of_switches,m_num_of_nodes);

        m_socket = new Socket*[m_num_of_nodes];

        m_server_socket.init(m_port);
        m_server_socket.registerEventListener(this);
        m_completed=0;
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
    if(strlen(message) >=5){
        if(strncmp("NODE ",message,5)==0){
            int nodeid;
            sscanf(message+5,"%d",&nodeid);
            cout << "RECV NODE ID " << nodeid << endl;
            m_logfile << "RECV NODE ID " << nodeid << endl;
            m_socket[nodeid-1] = socket;

            m_connected++;
            if(m_connected==m_num_of_nodes){
                cout << "SEND START SIGNAL" << endl;
                m_logfile << "SEND START SIGNAL" << endl;
                for(int i=0;i<m_num_of_nodes;i++){
                    m_socket[i]->send("START");
                }
            }


            return 0;
        }else if(strncmp("END ",message,4)==0){
            int nodeid;
            sscanf(message+4,"%d",&nodeid);
            m_completed++;
            cout << "RECV END NODE " << nodeid << endl;
            if(m_completed==m_num_of_nodes){
                cout << "SEND DISCONNECT SIGNAL" << endl;
                m_logfile << "SEND DISCONNECT SIGNAL" << endl;
                for(int i=0;i<m_num_of_nodes;i++){
                    m_socket[i]->send("DISCONNECT");
                }
            }
        }
    }

    //regular msg

    unsigned int from=0, to=0, timestamp=0;
    char buff[SOCKET_MAX_BUFFER_SIZE];
    memcpy(&to,&message[0],1);
    memcpy(&from,&message[1],1);
    memcpy(&timestamp,&message[2],4);
    bzero(buff,SOCKET_MAX_BUFFER_SIZE);
    memcpy(buff,&message[6],SOCKET_MAX_BUFFER_SIZE-6);

    //forward msg
    if(to>=1 && to <=m_num_of_nodes && m_socket[to-1] != 0){
        cout << "FORWARD from=" << from << " to=" << to << " timestamp=" << timestamp << " msg: " << buff << endl;
        m_logfile << "FORWARD from=" << from << " to=" << to << " timestamp=" << timestamp << " msg: " << buff << endl;
        this->m_socket[to-1]->send(message);
    }else{
        cout << "DISCARD from=" << from << " to=" << to << " timestamp=" << timestamp << " msg: " << buff << endl;
        m_logfile << "DISCARD from=" << from << " to=" << to << " timestamp=" << timestamp << " msg: " << buff << endl;
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
    if(m_logfile.is_open()){
        m_logfile.close();
    }
}
