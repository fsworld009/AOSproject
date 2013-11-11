#include "LAKNode.h"
#include <unistd.h>
#include <iostream>
using namespace std;

LAKNode::LAKNode(int node_id): Node(node_id)
{
    //ctor
    this->timer=0;

    //assume 1 has token
    if(node_id==1){
        this->has_token=true;
    }else{
        this->has_token=false;
    }
}

int LAKNode::receive(int from, int to, int timestamp, string message){
    return 0;
}

LAKNode::~LAKNode()
{
    //dtor
}

int LAKNode::run(){
	this->timer = 0;
	while (this->timer < 200)
	{

		timer++;
        cout << timer << endl;
		set<int>::iterator iter = time_schedule.find(timer);
		if (iter != time_schedule.end())
		{
			//this->send_request();
		}
		sleep(1);
	}

    return 0;
}
