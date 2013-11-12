#include "NodeNetwork.h"
#include "Node.h"
#include <string.h>
#include <iostream>
#include <string>
#include <stdio.h>
using namespace std;



NodeNetwork::NodeNetwork(Node* node, int node_id): m_node(node)
{
    //ctor
    //m_sockets=0;
    m_socket=0;

    m_port=0;
    m_num_of_nodes=0;


    m_socket=0;

    m_node_id = node_id;
}

int NodeNetwork::getHostName(int netid, char* host){
    if(netid==0){
        strcpy(host,"localhost");
    }else if(netid>=1 && netid<=9){
        sprintf(host,"net0%d.utdallas.edu",netid);
    }else{
        sprintf(host,"net%d.utdallas.edu",netid);
    }
    return 0;
}


int NodeNetwork::init(){
    //parse config file

    //work without switch
    FILE* fp = fopen("socket.txt","r");

    fscanf(fp,"%d",&m_port);
    fscanf(fp,"%d",&m_num_of_switches);
    //m_switch_netid = new int[m_num_of_switches];
    for(int i=0;i<m_num_of_switches;i++){
        fscanf(fp,"%d",&m_switch_netid);
        if(i==(m_node_id-1)/4){
            break;
        }
    }

    fscanf(fp,"%d",&m_num_of_nodes);
    /*m_node_netid = new int[m_num_of_nodes];
    for(int i=0;i<m_num_of_nodes;i++){
        fscanf(fp,"%d",&m_node_netid[i]);
    }*/

    fclose(fp);

    m_socket = new Socket();
    m_socket->registerEventListener(this);
    return 0;
}



int NodeNetwork::send(unsigned int from, unsigned int to, unsigned int timestamp, char* message){
    char buff[SOCKET_MAX_BUFFER_SIZE];
    bzero(buff,SOCKET_MAX_BUFFER_SIZE);
    memcpy(buff,&to,1);
    memcpy(buff+1,&from,1);
    memcpy(buff+2,&timestamp,4);
    memcpy(buff+6,message,SOCKET_MAX_BUFFER_SIZE-6);

    //unsigned int x1=0,x2=0,x3=0;
    //memcpy(&x1,buff,1);
    //memcpy(&x2,buff+1,1);
    //memcpy(&x3,buff+2,4);
    //cout << "RECOVER: " << x1 << " " << x2 << " " << x3 << endl;

    //cout << "aaaa" << endl;
    //sprintf(buff,"%u%u%4u",to,from,timestamp);

    //printf("STRLEN: %d RAW: %s\n",strlen(buff),buff);
    /*char* a = (char*)&timestamp;
    buff[0] = (char) to;
    buff[1] = (char) from;
    buff[2] = (char) *(a);
    buff[3] = (char) *(a+1);
    buff[4] = (char) *(a+2);
    buff[5] = (char) *(a+3);*/

    //strcpy(&buff[6],message);

    //cout << "RECOVER: " << x1 <<  " " << x2 << " " << x3 << " MSG: " << buff2 << endl;
    /*char buff2[1024];
    cout << "RAW: " << cout.hex << buff << endl;
    sscanf(buff,"%u%u%u",&x1,&x2,&x3);
    strcpy(buff2,buff+6);
    cout << "RECOVER: " << x1 << " " << x2 << " " << x3 << endl;*/

    //Shift offset by -1
    from = from-1;
    to = to-1;


    /*if(m_mode==0){
        //find socket to send
        if(m_sockets[to] == 0){

            m_sockets[to] = new Socket();

            char host[20];

            if(m_netid[to]<10){
                sprintf(host,"net0%d.utdallas.edu",m_netid[to]);
            }else{
                sprintf(host,"net%d.utdallas.edu",m_netid[to]);
            }

            m_sockets[to]->registerEventListener(this);

            m_sockets[to]->connectHost(host,m_port);

        }

        m_sockets[to]->send(buff);
    }else{
        //send to switch
        //m_socket->send(buff);
    }*/

    printf("NodeNetwork:: send from=%d to=%d timestamp=%d msg: %s\n",from+1,to+1,timestamp,message);
    m_socket->send(buff);
    return 0;
}

int NodeNetwork::start(){
    /*if(m_mode==0){
        m_server_socket.start();
    }else{
        //work with switch
        //m_socket = new Socket();
        //config this socket to connect to switch
    }*/
    char host[24];
    this->getHostName(m_switch_netid,host);
    m_socket->connectHost(host,m_port);
    return 0;
}

int NodeNetwork::close(){


    /*if(m_sockets !=0){
        for(int i=0;i<m_num_of_nodes;i++){
            if(m_sockets[i] != 0){
                m_sockets[i]->disconnect();
                delete m_sockets[i];
                m_sockets[i]=0;
            }
        }
        delete[] m_sockets;
        m_sockets=0;
    }*/

    if(m_socket != 0){
        m_socket->disconnect();
        delete m_socket;
        m_socket=0;

    }

    //m_server_socket.disconnect();
    return 0;
}

NodeNetwork::~NodeNetwork()
{
    //dtor
    close();
}
/*
int NodeNetwork::onAccept(Socket* socket){
    socket->registerEventListener(this);
    //m_socket = socket;
    if(m_mode==0){
        char ip[20];
        socket->getBoundedIp(ip);

        //parse ip into 4 ints
        int ip_num[4];
        sscanf(ip,"%d.%d.%d.%d",&ip_num[0],&ip_num[1],&ip_num[2],&ip_num[3]);


        //analize the last number of ip address to get net machine number
        //64~99 ->   X-63 (net01~36)
        //62 -> 36 (net37)
        //101~108 -> X-63 (net38~45)


        int netid;
        if(ip_num[3]==62){
            netid = 36;
        }else{
            netid = ip_num[3]-63;
        }
        printf("NodeNetwork:: acceptted socket is from net%d\n",netid);
        for(int i=0;i<m_num_of_nodes;i++){
            if(m_netid[i]==netid){
                m_sockets[i] = socket;
            }
        }

    }


    return 0;
}
int NodeNetwork::onDisconnect(ServerSocket* serverSocket){
    return 0;
}*/

int NodeNetwork::onConnect(Socket* socket){
    char buff[10];
    sprintf(buff,"NODE %d",m_node_id);
    socket->send(buff);
    return 0;
}
int NodeNetwork::onReceive(char* message,Socket* socket){
    /*int to, from, timestamp;
    char buff[SOCKET_MAX_BUFFER_SIZE];
    from = (int)message[0];
    to = (int)message[1];
    timestamp = (int) ((message[2]<<3)|(message[3]<<2)|(message[4]<<1)|(message[5]));

    strcpy(buff,message+6);*/

    unsigned int from=0,to=0,timestamp=0;
    memcpy(&to,message,1);
    memcpy(&from,message+1,1);
    memcpy(&timestamp,message+2,4);

    char buff[SOCKET_MAX_BUFFER_SIZE];
    bzero(buff,SOCKET_MAX_BUFFER_SIZE);
    memcpy(&buff,message+6,SOCKET_MAX_BUFFER_SIZE-6);

    printf("NodeNetwork:: recv from=%d to=%d timestamp=%d msg: %s\n",from,to,timestamp,buff);
    //m_node->receive(from,to,timestamp,buff);
    string msg_string(buff);
    m_node->receive_message(msg_string);







    return 0;
}
int NodeNetwork::onDisconnect(Socket* socket){
    /*if(m_mode==0){
        if(m_sockets != 0){
            for(int i=0;i<m_num_of_nodes;i++){
                if(m_sockets[i] != 0){
                    if(m_sockets[i]==socket){
                        delete m_sockets[i];
                        m_sockets[i] = 0;
                    }
                }
            }
        }
    }*/
    return 0;
}