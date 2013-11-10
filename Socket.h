/*
Yu-Chun Lee yxl122130
10.26.2013
a class that proovides abstraction of standard socket lib of UNIx (sys/socket.h)
this class only provides TCP socket
*/

#ifndef SOCKET_H
#define SOCKET_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <queue>
#include "Thread.h"
#include "MutexLock.h"

#define SOCKET_MAX_BUFFER_SIZE 256

class SocketEventListener;

class Socket
{
    public:
        Socket();
        Socket(int socket);
        virtual ~Socket();
        int connectHost(char* host, int port);
        int send(char* Message);
        bool registerEventListener(SocketEventListener* listener);
        bool unregisterEventListener();
        int disconnect();
        //static void* send_thread_main(void* args);
        //static void* receive_thread_main(void* args);
    protected:

    private:
        int m_socket;
        sockaddr_in m_addr;
        bool m_thread_running;
        int createThreads();
        //int receiveTask();
        //int sendTask();

        MutexLock m_queue_lock;
        int m_recv_node_no;
        char m_buffer[SOCKET_MAX_BUFFER_SIZE];
        std::queue<char*> m_messages;
        //pthread_t m_send_thread, m_receive_thread;

        class ReceiveThread: public Thread{
            public:
                ReceiveThread(Socket* parent);
                virtual int run();
            private:
                Socket* m_parent;
        };

        class SendThread: public Thread{
            public:
                SendThread(Socket* parent);
                virtual int run();
            private:
                Socket* m_parent;
        };

        SendThread m_send_thread;
        ReceiveThread m_receive_thread;


        SocketEventListener* m_event_listener;
};

#endif // SOCKET_H
