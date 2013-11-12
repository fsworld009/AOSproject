#include "Node.h"
#include <iostream>
#include <unistd.h>
#include "string.h"
#include <stdlib.h>
#include <fstream>
using namespace std;

Node::Node(int node_id): node_id(node_id), m_node_network(this, this->node_id)
{
    //ctor
    m_wait=true;
}

//called by NodeNetwork when it received "START" signal
int Node::signal(){
    m_wait_lock.lock();
    m_wait=false;
    m_wait_lock.unlock();
}

int Node::waitForSignal(){
    m_wait_lock.lock();
    cout << "Waiting for signal..." << endl;
    while(m_wait){
        m_wait_lock.unlock();
        usleep(100);
        m_wait_lock.lock();
    }
    m_wait_lock.unlock();
    return 0;
}

int Node::init(){
    parse_quorum();
    parse_schedule();


    //print set

    set<int>::iterator iter;
    for (iter = quorum_set.begin(); iter != quorum_set.end(); iter++)
    {
        cout << *iter << " ";
    }
    cout << endl;
    for (iter = time_schedule.begin(); iter != time_schedule.end(); iter++)
    {
        cout << *iter << " ";
    }
    cout << endl;

    m_node_network.init();
    return 0;
}

int Node::start(){
    m_node_network.start();
    waitForSignal();
    run();
    return 0;
}

int Node::send(unsigned int from,unsigned int to,unsigned int timestamp, string message){
    m_node_network.send(from,to,timestamp,(char*)message.c_str());
    return 0;
}

int Node::parse_quorum(){
    char filepath[20];
    sprintf(filepath,"quorum%d.txt",node_id);
	ifstream ifs(filepath, ios::in);
	string str;
	while (getline(ifs, str))
	{
		quorum_set.insert(atoi(str.c_str()));
	}
	return 0;
}


int Node::parse_schedule(){
    char filepath[20];
    sprintf(filepath,"schedule%d.txt",node_id);
	ifstream ifs(filepath, ios::in);
	string str;
	while (getline(ifs, str))
	{
		time_schedule.insert(atoi(str.c_str()));
	}
	return 0;
}

Node::~Node()
{
    //dtor

}
