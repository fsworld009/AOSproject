#include "Node.h"

Node::Node(): m_node_network(this)
{
    //ctor
}

int Node::send(int from, int to, int timestamp, char* message){
    m_node_network.send(from,to,timestamp,message);
    return 0;
}

Node::~Node()
{
    //dtor

}
