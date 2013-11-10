#include "Socket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

#include <string.h>
#include <iostream>
#include <unistd.h>
#include <netdb.h>
using namespace std;

#include "SocketEventListener.h"

Socket::Socket(): m_send_thread(this), m_receive_thread(this)
{
    //ctor
    m_socket = -1;
    m_event_listener = 0;

}

Socket::Socket(int socket): m_socket(socket), m_send_thread(this), m_receive_thread(this)
{
    //ctor
    m_event_listener = 0;
    createThreads();
}

int Socket::connectHost(char* host,int port){

    if(m_socket!=-1){
        //close connection
    }

    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0){
        cout << "Socket: create socket error" << endl;
        exit(-1);
    }

    if (host == 0) {
        cout << "Socket: no host address" << endl;
        exit(-1);
    }

    sockaddr_in server_addr;
    hostent *server = 0;

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server = gethostbyname(host);
    bcopy((char *)server->h_addr,(char *)&server_addr.sin_addr.s_addr,server->h_length);
    server_addr.sin_port = htons(port);


    if (connect(m_socket,(sockaddr *)&server_addr,sizeof(server_addr)) < 0){
        cout << "Socket: socket connect error" << endl;
        return 0;
    }else{
        cout <<"Succeed" << endl;

        createThreads();
        return 1;
    }
}

int Socket::createThreads(){
    //cout << "c" << endl;
    m_thread_running = true;
    m_send_thread.start();
    m_receive_thread.start();
    return 0;
}

int Socket::send(char* message){
    if(m_socket==-1){
        cout << "Socket: no connection yet" << endl;
    }
    m_queue_lock.lock();
    char* temp = new char[SOCKET_MAX_BUFFER_SIZE];
    strcpy(temp,message);
    m_messages.push(temp);
    m_queue_lock.unlock();
    return 1;

}

bool Socket::registerEventListener(SocketEventListener* listener){
    if(m_event_listener != 0){
        //already registered a event listener
        return false;
    }
    m_event_listener = listener;
    return true;
}

bool Socket::unregisterEventListener(){
    if(m_event_listener==0){
        //no event listener
        return false;
    }
    m_event_listener = 0;
    return true;
}

Socket::~Socket()
{
    //dtor


    //close socket
    disconnect();
}

int Socket::disconnect(){
    if(m_socket != -1){


        //close sockets (hence unblock send/recv threads)
        shutdown(m_socket,SHUT_RDWR);
        close(m_socket);
        m_socket = -1;

        //trun off threads
        m_thread_running = false;
        m_receive_thread.join();
        m_send_thread.join();

        //clear msg queue
        m_queue_lock.lock();
        while(!m_messages.empty()){
            delete[] m_messages.front();
            m_messages.pop();
        }
        m_queue_lock.unlock();
        cout << "Socket: disconnected" << endl;

    }
    if(m_event_listener != 0){
        m_event_listener->onDisconnect(this);
    }

    return 0;
}

Socket::SendThread::SendThread(Socket* parent){
    m_parent = parent;
}

int Socket::SendThread::run(){
    char* msg;
    int result;
    while(m_parent->m_thread_running){
            m_parent->m_queue_lock.lock();
            if(!m_parent->m_messages.empty()){
                msg = m_parent->m_messages.front();

                m_parent->m_messages.pop();
                result = write(m_parent->m_socket,msg,strlen(msg));
                if(result ==-1 || result == 0){
                    cout << "Socket: send thread close" << endl;
                    m_parent->disconnect();
                    return 0;
                }else{
                    cout << "Socket: send msg " << msg << endl;
                }


                //deallocate memory
                delete[] msg;
            }
            m_parent->m_queue_lock.unlock();
        usleep(100);
    }
    return 0;
}

Socket::ReceiveThread::ReceiveThread(Socket* parent){
    m_parent = parent;
}

int Socket::ReceiveThread::run(){
    int result;
    while(m_parent->m_thread_running){
        bzero(m_parent->m_buffer,SOCKET_MAX_BUFFER_SIZE);
        result = read(m_parent->m_socket,m_parent->m_buffer,SOCKET_MAX_BUFFER_SIZE-1);
        if(result ==-1 || result == 0){
            cout << "Socket: receive thread close" << endl;
            m_parent->disconnect();
            return 0;
        }else{
            cout << "Socket: receive msg " << m_parent->m_buffer << endl;
        }



        if(m_parent->m_event_listener != 0){
            m_parent->m_event_listener->onReceive(m_parent->m_buffer,m_parent);
        }
        usleep(100);
    }
    return 0;
}