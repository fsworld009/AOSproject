#ifndef LAKNODE_H
#define LAKNODE_H
#include "Node.h"
/*
Yu-Chun Lee & Jack Lai 11.1.2013
implementation of our alforithm design
*/



class LAKNode: public Node
{
    public:
        LAKNode(int node_id);
        virtual ~LAKNode();
        int virtual receive(int from, int to, int timestamp, char* message);
    protected:
    private:
};

#endif // LAKNODE_H
