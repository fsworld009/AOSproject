#include "ServerSocket.h"
#include "ServerSocketEventListener.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>
using namespace std;

ServerSocket::ServerSocket(): m_acceptThread(this)
{
    //ctor
    m_thread_running  =false;
    m_event_listener = 0;
    m_socket = -1;
}

int ServerSocket::init(int port){
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0){
        cout << "ServerSocket: Socket opening error" << endl;
        exit(-1);
    }
    bzero((char *) &m_addr, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    hostent *server = 0;
    server = gethostbyname("localhost");
    bcopy((char *)server->h_addr,
         (char *)&m_addr.sin_addr.s_addr,
         server->h_length);
    m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_addr.sin_port = htons(port);

    cout << "Addr: " << inet_ntoa(m_addr.sin_addr) << " Port: " << ntohs(m_addr.sin_port) << endl;
    return 0;
}

int ServerSocket::start(){
    int a = bind(m_socket, (struct sockaddr *) &m_addr,sizeof(m_addr));
    if (a < 0){
        cout << "ServerSocket: Socket bind error " << a << endl;
        exit(-1);
    }

    m_thread_running = true;
    m_acceptThread.start();

    return 0;
}

int ServerSocket::disconnect(){
    //close socket
    if(m_socket != -1){


        //close sockets (hence unblock send/recv threads)
        shutdown(m_socket,SHUT_RDWR);
        close(m_socket);
        m_socket = -1;

        //trun off threads
        m_thread_running = false;
        m_acceptThread.join();



        cout << "ServerSocket: closed" << endl;

        if(m_event_listener != 0){
            m_event_listener->onDisconnect(this);
        }

    }
    return 0;
}

//task for the listen thread
void *ServerSocket::thread_main(void *args) {
    ((ServerSocket*)args)->listenTask();
    return 0;
}

int ServerSocket::listenTask(){

        //usleep(1);
        return 0;
}

bool ServerSocket::registerEventListener(ServerSocketEventListener* listener){
    if(m_event_listener != 0){
        //already registered a event listener
        return false;
    }
    m_event_listener = listener;
    return true;
}

bool ServerSocket::unregisterEventListener(){
    if(m_event_listener==0){
        //no event listener
        return false;
    }
    m_event_listener = 0;
    return true;
}

ServerSocket::~ServerSocket()
{
    //dtor
    //
    //disconnect();

}

ServerSocket::AcceptThread::AcceptThread(ServerSocket* parent){
    m_parent = parent;
}

int ServerSocket::AcceptThread::run(){
    cout << "ServerSocket: Listen thread start"  << endl;
    sockaddr client_addr;
    socklen_t client_len = sizeof(client_addr);
    sockaddr_in* client_addr_in = (sockaddr_in*)&client_addr;
    int accept_socket;
    listen(m_parent->m_socket,3);
    while(m_parent->m_thread_running){
        accept_socket = accept(m_parent->m_socket, &client_addr, &client_len);
        getpeername(accept_socket,&client_addr, &client_len);

        if(accept_socket != -1){
            cout << "ServerSocket: accept socket " << accept_socket << " from " << inet_ntoa(client_addr_in->sin_addr) << endl;
            //temp, need improved
            //m_parent->m_accepted_socket = new Socket(accept_socket);

            //upper listener is responsible for memory management of this Socket
            if(m_parent->m_event_listener != 0){
                m_parent->m_event_listener->onAccept(new Socket(accept_socket,inet_ntoa(client_addr_in->sin_addr)));
            }

        }
        usleep(100);
    }
    return 0;
}
