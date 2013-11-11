#ifndef LAKNODE_H
#define LAKNODE_H
#include "Node.h"
/*
Yu-Chun Lee & Jack Lai 11.1.2013
implementation of our alforithm design
*/

enum MessageType
{
	REQUEST,
	RELEASE,
	RECEIVE,
	ACK,
	RELAY,
	TOKEN
};

struct Message
{
	MessageType type;
	int from;
	int to;
	int seq;
	int relay;
	set<Message> req_list;

	bool operator < (const Message& msg) const
	{
		if (this->seq < msg.seq)
			return true;
		else if ((this->seq == msg.seq) && (this->from < msg.from))
			return true;
		else
			return false;
	}
};

class LAKNode: public Node
{
    public:
        LAKNode(int node_id);
        virtual ~LAKNode();
        int virtual receive(int from, int to, int timestamp, string message);

    protected:
    private:
        int virtual run();
        int token_holder;
        int	expt_resp;
        bool is_inCS;
        bool has_token;
        int	timer;
        set<int> acked_node;
        set<Message> token_list;



        //mutual exclusion
        void	receive_message(string msgstr);
        void	receive_request(Message msg);
        void	receive_ack(Message msg);
        void	receive_release(Message msg);
        void	receive_receive(Message msg);
        void	receive_relay(Message msg);
        void	receive_token(Message msg);
        void	send_request();

        //actions
        void	send_token(int to);
        void	send_release();
        void	send_receive();
        void	send_relay(int from, int seq);
        void	send_ack(int to);
        void	accessCS();
        void	finishCS();

        string	message_string(Message msg);
        Message string_message(string msgstr);

};

#endif // LAKNODE_H
