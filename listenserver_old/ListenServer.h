#ifndef LISTENSERVER_H
#define LISTENSERVER_H
#include "../thread/Thread.h"
#include "../thread/MutexLock.h"
#include <linux/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#define LSERVER_PORT 4567
#define LSERVER_BUFFER_SIZE 1027
using namespace std;

enum ROLE{NODE,SWITCH};


class ListenServer
{
    public:
        ListenServer();
        virtual ~ListenServer();
        int start();
        //int close_me();
        int set_end();
    protected:
    private:
        bool m_end;
        int get_netid();
        ROLE get_role(int netid);
        MutexLock m_lock;
        int m_server_sock;
        class NodeThread: public Thread
        {
            public:
                NodeThread(ListenServer* ls,int server_sock);
                virtual int run();
                int disconnect();
            private:
                ListenServer* m_ls;
                int m_client_sock;
        };
        
        NodeThread* m_node_thread;
};

#endif // ECHOSERVER_H
