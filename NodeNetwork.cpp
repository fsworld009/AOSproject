#include "NodeNetwork.h"
#include <stdio.h>

NodeNetwork::NodeNetwork(Node* node): m_node(node)
{
    //ctor
    m_sockets=0;
    m_socket=0;
}

NodeNetwork::NodeNetwork()
{
    //ctor

}


int NodeNetwork::init(){
    //parse config file
    FILE* fp = fopen("socket","r");

    fscanf(fp,"%d",&m_mode);
    fscanf(fp,"%d",&m_port);
    fscanf(fp,"%d",&m_num_of_nodes);


    m_netid = new int[m_num_of_nodes];

    printf("mode=%d port=%d num_of_nodes=%d\n",m_mode,m_port,m_num_of_nodes);

    //parse config file end
    //m_server_socket.init()

    if(m_mode==0){
        //work without switch
        m_sockets = new Socket*[m_num_of_nodes];

        for(int i=0;i<m_num_of_nodes;i++){
            m_sockets[i] = 0;   //init pointers to Socket
            fscanf(fp,"%d",&m_netid[i]);
            printf("m_netid[%d]=%d\n",i,m_netid[i]);
        }
        m_server_socket.init(m_port);
        m_server_socket.registerEventListener(this);
        m_server_socket.start();
    }else{
        //work with switch
        m_socket = new Socket();
        //config this socket to connect to switch
    }

    fclose(fp);
    return 0;
}

int NodeNetwork::send(int from, int to, int timestamp, char* message){
    //Shift offset by -1
    from = from-1;
    to = to-1;
    char buff[SOCKET_MAX_BUFFER_SIZE];
    sprintf(buff,"%c%c%d%s",from,to,timestamp,message);

    if(m_mode==0){
        //find socket to send
    }else{
        //send to switch
        //m_socket->send(buff);
    }


    return 0;
}

int NodeNetwork::start(){
    init();
    return 0;
}

NodeNetwork::~NodeNetwork()
{
    //dtor
    if(m_sockets !=0){
        for(int i=0;i<m_num_of_nodes;i++){
            if(m_sockets[i] != 0){
                delete m_sockets[i];
            }
        }
    }
    delete[] m_sockets;
}

int NodeNetwork::onAccept(Socket* socket){
    socket->registerEventListener(this);
    return 0;
}
int NodeNetwork::onDisconnect(ServerSocket* serverSocket){
    return 0;
}

int NodeNetwork::onConnect(Socket* socket){
    return 0;
}
int NodeNetwork::onReceive(char* message,Socket* socket){
    int from, to, timestamp;

    delete[] message;
    return 0;
}
int NodeNetwork::onDisconnect(Socket* socket){
    return 0;
}
