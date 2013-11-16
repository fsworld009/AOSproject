#include "MAKNode.h"
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>
using namespace std;


//////************************************************/////////////////////////////
//question: if a process has already initiated a CS request, can it initiates another when the first one hasn't been satisfied?
////////*********************************************////////////////////////////

MAKNode::MAKNode(int node_id) : Node(node_id)
{
	//ctor
	this->timer = 0;
	this->is_locked = false;
	this->is_inCS = false;
	this->seq = 1;
	this->locked_for = make_pair(-1,-1);		//locked_for(seq#, node#)
	this->waiting_queue.clear();				//for the requests haven't been processed
	this->response_map.clear();					//collect the feedbacks of other nodes in the same quorum
}

MAKNode::~MAKNode()
{
	//dtor
}

//initiate requests according to the time schedule
int MAKNode::run(){
	this->timer = 0;
	this->CS_timer=0;
    string recv_message;
    
	while (this->timer < 300)
	{
        //handle receive messages
        while(get_message(&recv_message)){
            receive_message(recv_message);
        }
        
        
		this->timer++;
		if(CS_timer==0 && is_inCS){
            finishCS();
		}else if(CS_timer>0){
            CS_timer--;
		}
		//cout << CS_timer << endl;
		set<int>::iterator iter = time_schedule.find(this->timer);
		if (iter != time_schedule.end())
		{
			this->send_request();
		}
		usleep(1000);
	}
    cout << "END" << endl;
    return 0;
}

//Message Handlers
void MAKNode::send_request()
{
	if (this->is_locked)
	{
		//can the node do anything else if it is in the CS??????????
		printf("Node %d is currently locked. Can't request CS now.\n", this->node_id);
		MAKNode::Message msg;
		msg.from = this->node_id;
		msg.seq = this->seq++;
		msg.type = REQUEST;
		this->waiting_queue.insert(msg);
		return;
	}

	MAKNode::Message msg;
	msg.from = this->node_id;
	msg.seq = this->seq++;
	msg.type = REQUEST;

	this->is_locked = true;
	this->locked_for = make_pair(msg.seq, msg.from);
	this->waiting_queue.insert(msg);

	set<int>::iterator iter;
	for (iter = quorum_set.begin(); iter != quorum_set.end(); iter++)
	{
		if ((*iter) != node_id)
		{
			msg.to = (*iter);
			send(this->node_id, (*iter), timer, message_string(msg));
		}
	}
}

int MAKNode::receive_message(string msgstr){

	MAKNode::Message msg = string_message(msgstr);

	switch (msg.type)
	{
	case REQUEST:
		receive_request(msg);
		break;
	case RELEASE:
		receive_release(msg);
		break;
	case LOCK:
		receive_locked(msg);
		break;
	case FAIL:
		receive_failed(msg);
		break;
	case INQUIRE:
		receive_inquiry(msg);
		break;
	case RELINQUISH:
		receive_relinquish(msg);
		break;
	default:
		printf("Unexpected message received!\n");
		break;
	}
	return 0;
}

void MAKNode::receive_request(MAKNode::Message msg)
{
	this->waiting_queue.insert(msg);

	if (this->is_locked)
	{
		MAKNode::Message first = *this->waiting_queue.begin();
		if ((first.from == msg.from) && (first.to == msg.to) && (first.seq == msg.seq))
			send_inquiry(this->locked_for.second);
		else
			send_failed(msg.from);
	}
	else
	{
		this->is_locked = true;
		this->locked_for = make_pair(msg.seq, msg.from);
		send_locked(msg.from);
	}
}

void MAKNode::receive_locked(MAKNode::Message msg)
{
	this->response_map.insert(make_pair(msg.from, msg.type));
	if (this->response_map.size() == this->quorum_set.size() - 1)
	{
		map<int, MessageType>::iterator iter;
		for (iter = response_map.begin(); iter != response_map.end(); iter++)
		{
			//if there is a fail msg in all the quorum response, just do nothing and return
			if ((*iter).second == FAIL)
				return;
		}

		this->accessCS();
		//this->finishCS();
	}
}

void MAKNode::receive_inquiry(MAKNode::Message msg)
{
	//if the process is in CS, just ignore the inquiry.
	if (this->is_inCS = true)
		return;

	//if the process is not in CS, first we check whether there is any fail response.
	map<int, MessageType>::iterator iter;
	for (iter = response_map.begin(); iter != response_map.end(); iter++)
	{
		if ((*iter).second == FAIL)
		{
			send_relinquish(msg.from);
			return;
		}
	}

	//if there is no fail response and the process is not in CS, then it means there is still some responses need to be received. We just save the inquiry.
	this->inquiry_list.insert(msg);
}

void MAKNode::receive_failed(MAKNode::Message msg)
{
	this->response_map.insert(make_pair(msg.from, msg.type));
	
	set<MAKNode::Message>::iterator iter;
	for (iter = this->inquiry_list.begin(); iter != this->inquiry_list.end(); iter ++)
	{
		send_relinquish((*iter).from);
	}

	this->inquiry_list.clear();
}

void MAKNode::receive_relinquish(MAKNode::Message msg)
{
	Message first = *this->waiting_queue.begin();
	this->locked_for = make_pair(first.seq, first.from);
	this->is_locked = true;
	send_locked(first.from);
}

void MAKNode::receive_release(MAKNode::Message msg)
{
	set<MAKNode::Message>::iterator iter;
	for (iter = this->waiting_queue.begin(); iter != this->waiting_queue.end(); iter++)
	{
		if ((*iter).from == msg.from)
			this->waiting_queue.erase(iter);
	}
	if (this->waiting_queue.empty())
	{
		this->is_locked = false;
	}
	else
	{
		send_locked(this->waiting_queue.begin()->from);
	}
}

//actions
void MAKNode::send_locked(int to)
{
	MAKNode::Message msg;
	msg.from = this->node_id;
	msg.to = to;
	msg.type = LOCK;

	send(this->node_id, to, timer, message_string(msg));
}

void MAKNode::send_failed(int to)
{
	MAKNode::Message msg;
	msg.from = this->node_id;
	msg.to = to;
	msg.type = FAIL;

	send(this->node_id, to, timer, message_string(msg));
}

void MAKNode::send_inquiry(int to)
{
	MAKNode::Message msg;
	msg.from = this->node_id;
	msg.to = to;
	msg.type = INQUIRE;

	send(this->node_id, to, timer, message_string(msg));
}

void MAKNode::send_relinquish(int to)
{
	MAKNode::Message msg;
	msg.from = this->node_id;
	msg.to = to;
	msg.type = RELINQUISH;

	send(this->node_id, to, timer, message_string(msg));
}

void MAKNode::send_release(int to)
{
	MAKNode::Message msg;
	msg.from = this->node_id;
	msg.to = to;
	msg.type = RELEASE;

	send(this->node_id, to, timer, message_string(msg));
}

void MAKNode::accessCS()
{
    this->is_inCS = true;
    this->CS_timer = CS_time;
	cout << "The node is entering CS now ..." << endl;
    send_access_cs_msg();
}

void MAKNode::finishCS()
{
	cout << "The node is exiting CS now ..." << endl;
    send_finish_cs_msg();
    this->is_locked = false;
	this->response_map.clear();
	this->locked_for = make_pair(-1, -1);
	this->waiting_queue.erase(this->waiting_queue.begin());

	set<int>::iterator iter;
	for (iter = this->quorum_set.begin(); iter != this->quorum_set.end(); iter++)
	{
		if ((*iter) != this->node_id)
		{
			send_release((*iter));
		}
	}
    
    this->is_inCS = false;
}

string MAKNode::message_string(Message msg)
{
	string str = "";
	stringstream ss;
	
	switch (msg.type)
	{
	case REQUEST:
		ss << msg.type << " " << msg.from << " " << msg.to << " " << msg.seq;
		str = ss.str();
		break;
	default:
		ss << msg.type << " " << msg.from << " " << msg.to;
		str = ss.str();
		break;
	}
	return str;
}

MAKNode::Message MAKNode::string_message(string msgstr)
{
	Message msg;
	string type = msgstr.substr(0, msgstr.find_first_of(" "));
	msg.type = (MessageType) atoi(type.c_str());
	msgstr = msgstr.substr(msgstr.find_first_of(" ") + 1);
	switch (msg.type)
	{
	case REQUEST:
		msg.from = atoi(msgstr.substr(0, msgstr.find_first_of(" ")).c_str());
		msgstr = msgstr.substr(msgstr.find_first_of(" ") + 1);

		msg.to = atoi(msgstr.substr(0, msgstr.find_first_of(" ")).c_str());
		msgstr = msgstr.substr(msgstr.find_first_of(" ") + 1);

		msg.seq = atoi(msgstr.c_str());
		break;
	default:
		msg.from = atoi(msgstr.substr(0, msgstr.find_first_of(" ")).c_str());
		msgstr = msgstr.substr(msgstr.find_first_of(" ") + 1);

		msg.to = atoi(msgstr.c_str());
		break;
	}
	return msg;
}
