#include "ListenServer.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

ListenServer::ListenServer()
{
    m_end=false;
    
    //ctor
    //m_port=LSERVER_PORT;
}

int ListenServer::set_end(){
    //m_lock.lock();
    printf("called\n");
    m_end = true;
    shutdown(m_server_sock,SHUT_RDWR);
    close(m_server_sock);
    //m_lock.unlock();

}

int ListenServer::start(){
    cout << "ListenServerewrew Start" << endl;
    printf("Start listening server\n");
    int netid = get_netid();
    printf("My netid is %d\n",netid);
    ROLE role = get_role(netid);
    
    
    struct sockaddr_in server_addr/*, client_addr*/;
	unsigned int server_sock, client_sock, ids;
	int addr_len, port, net_status;
    
	port = LSERVER_PORT;

    int option=1;
    setsockopt(server_sock,SOL_SOCKET,SO_REUSEADDR,(char*)&option,sizeof(option));
    server_sock= socket(AF_INET, SOCK_STREAM, 0);;
	 
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
 
 

    //printf("here");
    if (listen(server_sock, 100) != 0)
	{
		printf("Failed to listen to port %d", port);
		return 1;
	}else{
        printf("ListenServer:: start to listen to port %d\n",port);
    }
    
    sockaddr client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    //int client_sock;
    m_server_sock = server_sock;
    //m_lock.lock();
    while(!m_end){
        //m_lock.unlock();
        client_sock = accept(server_sock, &client_addr, &client_len);
        printf("ListenServer:: accept connection\n");
        if(client_sock>0){
            m_node_thread = new NodeThread(this,client_sock);
            m_node_thread->start();
        }
        //m_lock.lock();
    }
    //m_lock.unlock();
    
    return 0;
}

ListenServer::~ListenServer()
{
    //dtor

}
/*
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
    

}*/

int ListenServer::get_netid(){
    int netid=0;
    FILE *cmd=popen("uname -a | cut -d \" \" -f2","r"); //execute Linux command to get "netXX.utdallas.edu"
    char result[50];
    fgets(result, sizeof(result), cmd);
    char num[3];
    num[0]=result[3];
    num[1]=result[4];
    num[2]='\0';        //num="XX", XX is the net id
    netid = atoi(num);  //convert the num string into int
    pclose(cmd);
    return netid;
}

ROLE ListenServer::get_role(int netid){
    if(netid%5==0){
        return SWITCH;
    }else{
        return NODE;
    }
}

ListenServer::NodeThread::NodeThread(ListenServer* ls,int client_sock){
    m_ls = ls;
    m_client_sock = client_sock;
}

int ListenServer::NodeThread::run(){
    
    char buff[LSERVER_BUFFER_SIZE];
    bzero(buff,LSERVER_BUFFER_SIZE);
    recv(m_client_sock, buff, LSERVER_BUFFER_SIZE, 0);
    cout << buff << endl;
    if(strncmp(buff,"EXIT",4)==0){
        m_ls->set_end();
    }
    //do stuff()
    close(m_client_sock);
    delete this;
}