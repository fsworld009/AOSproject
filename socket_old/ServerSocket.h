#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H
#include <sys/socket.h>
#include <netinet/in.h>
#include "../thread/Thread.h"
#include "Socket.h"


/*
Yu-Chun Lee yxl122130
10.26.2013
a class that proovides abstraction of standard socket lib of UNIx (sys/socket.h)
this class only provides TCP server socket
*/

class ServerSocketEventListener;

class ServerSocket
{
    public:
        ServerSocket();
        int init(int port);
        int start();
        virtual ~ServerSocket();
        static void* thread_main(void* args);
        bool registerEventListener(ServerSocketEventListener* listener);
        bool unregisterEventListener();
        int disconnect();

    protected:
    private:
        int listenTask();
        int m_socket;

        bool m_thread_running;
        sockaddr_in m_addr;


        class AcceptThread: public Thread{
            public:
                AcceptThread(ServerSocket* parent);
                virtual int run();
            private:
                ServerSocket* m_parent;
        };

        AcceptThread m_acceptThread;

        //test only
        //Socket* m_accepted_socket;
        ServerSocketEventListener* m_event_listener;
};

#endif // SERVERSOCKET_H
