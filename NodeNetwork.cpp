#include "NodeNetwork.h"
#include <stdio.h>

//mode 0= without switches
//mode 1 = work with switches

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
    char buff[SOCKET_MAX_BUFFER_SIZE];
    sprintf(buff,"%c%c%4c%s",(char)from,(char)to,timestamp,message);

    //Shift offset by -1
    from = from-1;
    to = to-1;


    if(m_mode==0){
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
    if(m_mode==0){
        char ip[20];
        socket->getBoundedIp(ip);

        //parse ip into 4 ints
        int ip_num[4];
        sscanf(ip,"%d.%d.%d.%d",&ip_num[0],&ip_num[1],&ip_num[2],&ip_num[3]);

        /*analize the last number of ip address to get net machine number
        64~99 ->   X-64 (net01~36)
        62 -> 36 (net37)
        101~108 -> X-64 (net38~45)

        */
        int netid;
        if(ip_num[3]==62){
            netid = 36;
        }else{
            netid = ip_num[3]-64;
        }
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
}

int NodeNetwork::onConnect(Socket* socket){
    return 0;
}
int NodeNetwork::onReceive(char* message,Socket* socket){
    int from, to, timestamp;
    from = (int)message[0];
    to = (int)message[1];
    timestamp = (int) ((message[2]<<3)|(message[3]<<2)|(message[4]<<1)|(message[5]));

    delete[] message;
    //m_node->receive(from,to,timestamp,message);
    return 0;
}
int NodeNetwork::onDisconnect(Socket* socket){
    return 0;
}
