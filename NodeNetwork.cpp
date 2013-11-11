#include "NodeNetwork.h"
#include <string.h>
#include <stdio.h>



NodeNetwork::NodeNetwork(Node* node): m_node(node)
{
    //ctor
    m_sockets=0;
    m_socket=0;

    m_port=0;
    m_num_of_nodes=0;


    //mode 0= without switches
    //mode 1 = work with switches
    m_mode=0;
}




int NodeNetwork::init(){
    //parse config file

    //parse config file end
    //m_server_socket.init()


    if(m_mode==0){
            //work without switch
        FILE* fp = fopen("socket","r");

        fscanf(fp,"%d",&m_port);
        fscanf(fp,"%d",&m_num_of_nodes);


        m_netid = new int[m_num_of_nodes];

        printf("mode=%d port=%d num_of_nodes=%d\n",m_mode,m_port,m_num_of_nodes);

        m_sockets = new Socket*[m_num_of_nodes];

        for(int i=0;i<m_num_of_nodes;i++){
            m_sockets[i] = 0;   //init pointers to Socket
            fscanf(fp,"%d",&m_netid[i]);
            printf("m_netid[%d]=%d\n",i,m_netid[i]);
        }
        m_server_socket.init(m_port);
        m_server_socket.registerEventListener(this);
        m_server_socket.start();

        fclose(fp);
    }else{
        //work with switch
        //m_socket = new Socket();
        //config this socket to connect to switch
    }


    return 0;
}



int NodeNetwork::send(int from, int to, int timestamp, char* message){
    char buff[SOCKET_MAX_BUFFER_SIZE];
    sprintf(buff,"%c%c%4c%s",(char)to,(char)from,timestamp,message);

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
        printf("NodeNetwork:: send from=%d to=%d timestamp=%d msg: %s\n",from,to,timestamp,message);
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

int NodeNetwork::close(){


    if(m_sockets !=0){
        for(int i=0;i<m_num_of_nodes;i++){
            if(m_sockets[i] != 0){
                m_sockets[i]->disconnect();
                delete m_sockets[i];
                m_sockets[i]=0;
            }
        }
        delete[] m_sockets;
        m_sockets=0;
    }

    if(m_socket != 0){
        m_socket->disconnect();
        delete m_socket;
        m_socket=0;

    }

    m_server_socket.disconnect();
    return 0;
}

NodeNetwork::~NodeNetwork()
{
    //dtor
    close();
}

int NodeNetwork::onAccept(Socket* socket){
    socket->registerEventListener(this);
    //m_socket = socket;
    if(m_mode==0){
        char ip[20];
        socket->getBoundedIp(ip);

        //parse ip into 4 ints
        int ip_num[4];
        sscanf(ip,"%d.%d.%d.%d",&ip_num[0],&ip_num[1],&ip_num[2],&ip_num[3]);


        /*analize the last number of ip address to get net machine number
        64~99 ->   X-63 (net01~36)
        62 -> 36 (net37)
        101~108 -> X-63 (net38~45)

        */
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
}

int NodeNetwork::onConnect(Socket* socket){
    return 0;
}
int NodeNetwork::onReceive(char* message,Socket* socket){
    int to, from, timestamp;
    char buff[SOCKET_MAX_BUFFER_SIZE];
    from = (int)message[0];
    to = (int)message[1];
    timestamp = (int) ((message[2]<<3)|(message[3]<<2)|(message[4]<<1)|(message[5]));

    strcpy(buff,message+6);
    printf("NodeNetwork:: recv from=%d to=%d timestamp=%d msg: %s\n",from,to,timestamp,buff);
    //m_node->receive(from,to,timestamp,message);


    delete[] message;

    return 0;
}
int NodeNetwork::onDisconnect(Socket* socket){
    if(m_mode==0){
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
    }
    return 0;
}
