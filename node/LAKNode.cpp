#include "LAKNode.h"
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>
using namespace std;

LAKNode::LAKNode(int node_id): Node(node_id)
{
    //ctor
    this->timer=0;

    //assume 1 has token
    if(node_id==1){
        this->token_holder = 1;
        this->seq = -1;
        this->expt_resp = -1;
        this->is_inCS = false;
        this->has_token = true;
    }else{
        this->token_holder = 1;
        this->seq = -1;
        this->expt_resp = -1;
        this->is_inCS = false;
        this->has_token = false;
    }
}

LAKNode::~LAKNode()
{
    //dtor
}

int LAKNode::run(){
	this->timer = 0;
	while (this->timer < 300)
	{

		this->timer++;
		//cout << timer << endl;
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
void LAKNode::send_request()
{
	if (this->has_token)
	{
		this->is_inCS = true;
		accessCS();
		this->is_inCS = false;
		finishCS();
	}
	else
	{
		LAKNode::Message msg;
		msg.from = this->node_id;
		msg.seq = this->seq++;
		msg.type = REQUEST;

		set<int>::iterator iter = quorum_set.find(token_holder);
		if (iter != quorum_set.end())
		{
			msg.to = token_holder;
			send(this->node_id, token_holder, timer, message_string(msg));
		}
		else
		{
			for (iter = quorum_set.begin(); iter != quorum_set.end(); iter++)
			{
				if ((*iter) != node_id)
				{
					msg.to = (*iter);
					send(this->node_id, (*iter), timer, message_string(msg));
				}
			}
		}

		has_token = false;
		is_inCS = false;
		acked_node.clear();
	}
}

int LAKNode::receive_message(string msgstr){

	LAKNode::Message msg = string_message(msgstr);

	switch (msg.type)
	{
	case REQUEST:
		receive_request(msg);
		break;
	case RELEASE:
		receive_release(msg);
		break;
	case ACK:
		receive_ack(msg);
		break;
	case RECEIVE:
		receive_receive(msg);
		break;
	case RELAY:
		receive_relay(msg);
		break;
	case TOKEN:
		receive_token(msg);
		break;
	default:
		break;
	}
	return 0;
}

void LAKNode::receive_request(LAKNode::Message msg)
{
	if (has_token)
	{
		if (is_inCS)
		{
			token_list.insert(msg);
		}
		else
		{
			send_token(msg.from);
			send_release();
		}
	}
	else
	{
		if (token_holder == -1)
		{
			send_ack(msg.from);
		}
		else
		{
			send_relay(msg.from, msg.seq);
		}
	}
}

void LAKNode::receive_ack(LAKNode::Message msg)
{
	acked_node.insert(msg.from);
	expt_resp--;
}

void LAKNode::receive_receive(LAKNode::Message msg)
{
	this->token_holder = msg.from;
}

void LAKNode::receive_release(LAKNode::Message msg)
{
	this->token_holder = -1;
}

void LAKNode::receive_relay(LAKNode::Message msg)
{

}

void LAKNode::receive_token(LAKNode::Message msg)
{
	if (msg.req_list.begin()->from != this->node_id)
	{
		send_token(msg.from);
	}
	else
	{
		has_token = true;
		cout << "T1" << endl;
		set<int>::iterator iter;
		for (iter = quorum_set.begin(); iter != quorum_set.end(); iter++)
		{
			if ((*iter) != this->node_id)
				send_receive();
		}
		is_inCS = true;
		accessCS();
		is_inCS = false;
		finishCS();
	}
}


//actions
void LAKNode::send_token(int to)
{
	LAKNode::Message msg;
	msg.from = this->node_id;
	msg.to = to;
	msg.type = TOKEN;
	msg.req_list = this->token_list;

	send(this->node_id, to, timer, message_string(msg));
}

void LAKNode::send_ack(int to)
{
	LAKNode::Message msg;
	msg.from = this->node_id;
	msg.to = to;
	msg.type = ACK;

	send(this->node_id, to, timer, message_string(msg));
}

void LAKNode::send_release()
{
	set<int>::iterator iter;
	for (iter = quorum_set.begin(); iter != quorum_set.end(); iter++)
	{
		if ((*iter) != this->node_id)
		{
			LAKNode::Message msg;
			msg.from = this->node_id;
			msg.to = (*iter);
			msg.type = RELEASE;

			send(this->node_id, (*iter), timer, message_string(msg));
		}
	}
}

void LAKNode::send_receive()
{
	set<int>::iterator iter;
	for (iter = quorum_set.begin(); iter != quorum_set.end(); iter++)
	{
		if ((*iter) != this->node_id)
		{
			LAKNode::Message msg;
			msg.from = this->node_id;
			msg.to = (*iter);
			msg.type = RECEIVE;

			send(this->node_id, (*iter), timer, message_string(msg));
		}
	}
}


void LAKNode::send_relay(int from, int seq)
{
	LAKNode::Message msg;
	msg.from = from;
	msg.to = this->token_holder;
	msg.type = RELAY;
	msg.relay = this->node_id;
	msg.seq = seq;
	send(this->node_id, this->token_holder, timer, message_string(msg));
}

void LAKNode::accessCS()
{
	cout << "The node is entering CS now ..." << endl;
	for (size_t i = 0; i < 100; i++)
	{
	}
	cout << "The node is exiting CS now ..." << endl;
}

void LAKNode::finishCS()
{
	if (token_list.empty() != true)
	{
		has_token = false;
		send_token(token_list.begin()->from);
		send_release();
	}
}

string LAKNode::message_string(Message msg)
{
	string str="";
	stringstream ss;
	switch (msg.type)
	{
	case REQUEST:
		ss << msg.type << " " << msg.from << " " << msg.to << " " << msg.seq;
		str = ss.str();
		break;
	case RELEASE:
		ss << msg.type << " " << msg.from << " " << msg.to;
		str = ss.str();
		break;
	case ACK:
		ss << msg.type << " " << msg.from << " " << msg.to;
		str = ss.str();
		break;
	case RECEIVE:
		ss << msg.type << " " << msg.from << " " << msg.to;
		str = ss.str();
		break;
	case RELAY:
		ss << msg.type << " " << msg.from << " " << msg.to << " " << msg.seq << " " << msg.relay;
		str = ss.str();
		break;
	case TOKEN:
		ss << msg.type << " " << msg.from << " " << msg.to;
		for (set<Message>::iterator iter = msg.req_list.begin(); iter != msg.req_list.end(); iter++)
		{
			ss << " " << message_string(*iter);
		}
		str = ss.str();
		break;
	default:
		break;
	}
	return str;
}

LAKNode::Message LAKNode::string_message(string msgstr)
{
	Message msg;
	string type = msgstr.substr(0,msgstr.find_first_of(" "));
	msg.type = (MessageType)atoi(type.c_str());
	msgstr = msgstr.substr(msgstr.find_first_of(" ")+1);
	//set<Message> req_list;
	switch (msg.type)
	{
	case REQUEST:
		msg.from = atoi(msgstr.substr(0, msgstr.find_first_of(" ")).c_str());
		msgstr = msgstr.substr(msgstr.find_first_of(" ") + 1);

		msg.to = atoi(msgstr.substr(0, msgstr.find_first_of(" ")).c_str());
		msgstr = msgstr.substr(msgstr.find_first_of(" ") + 1);

		msg.seq = atoi(msgstr.c_str());
		break;
	case RELEASE:
		msg.from = atoi(msgstr.substr(0, msgstr.find_first_of(" ")).c_str());
		msgstr = msgstr.substr(msgstr.find_first_of(" ") + 1);

		msg.to = atoi(msgstr.c_str());
		break;
	case ACK:
		msg.from = atoi(msgstr.substr(0, msgstr.find_first_of(" ")).c_str());
		msgstr = msgstr.substr(msgstr.find_first_of(" ") + 1);

		msg.to = atoi(msgstr.c_str());
		break;
	case RECEIVE:
		msg.from = atoi(msgstr.substr(0, msgstr.find_first_of(" ")).c_str());
		msgstr = msgstr.substr(msgstr.find_first_of(" ") + 1);

		msg.to = atoi(msgstr.c_str());
		break;
	case RELAY:
		msg.from = atoi(msgstr.substr(0, msgstr.find_first_of(" ")).c_str());
		msgstr = msgstr.substr(msgstr.find_first_of(" ") + 1);

		msg.to = atoi(msgstr.substr(0, msgstr.find_first_of(" ")).c_str());
		msgstr = msgstr.substr(msgstr.find_first_of(" ") + 1);

		msg.seq = atoi(msgstr.substr(0, msgstr.find_first_of(" ")).c_str());
		msgstr = msgstr.substr(msgstr.find_first_of(" ") + 1);

		msg.relay = atoi(msgstr.c_str());
		break;
	case TOKEN:
		msg.from = atoi(msgstr.substr(0, msgstr.find_first_of(" ")).c_str());
		msgstr = msgstr.substr(msgstr.find_first_of(" ") + 1);

		msg.to = atoi(msgstr.substr(0, msgstr.find_first_of(" ")).c_str());
		msgstr = msgstr.substr(msgstr.find_first_of(" ") + 1);

		if (msgstr.length() != 0)
		{
			Message rq;
			rq.from = atoi(msgstr.substr(0, msgstr.find_first_of(" ")).c_str());
			msgstr = msgstr.substr(msgstr.find_first_of(" ") + 1);

			rq.to = atoi(msgstr.substr(0, msgstr.find_first_of(" ")).c_str());
			msgstr = msgstr.substr(msgstr.find_first_of(" ") + 1);

			rq.seq = atoi(msgstr.substr(0, msgstr.find_first_of(" ")).c_str());
			msgstr = msgstr.substr(msgstr.find_first_of(" ") + 1);

			msg.req_list.insert(rq);
		}
		//msg.req_list = req_list;
		break;
	default:
		break;
	}
    return msg;
}
