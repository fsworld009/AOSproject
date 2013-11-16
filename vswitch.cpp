#include <stdio.h>
#include <string.h>
#include <linux/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <iostream>
#include <fstream>

#define PEND_CONNECTIONS (100)
using namespace std;

typedef struct req
		//each process will only be handling messages from a single node
		//get a list of when to expect messages and whether to drop them
{
	struct req* next;
	struct req* prev;
	int time;
	int drop;
} req;

int forward (char* msg, char* to_addr)
{
	int num = 6789;	
	int sockfd;
	struct addrinfo *result;
	struct sockaddr_in *server_addr;
    	
	int err = getaddrinfo(to_addr, NULL, NULL, &result);
	if( err != 0)
	{
		fprintf(stderr, "\ngetaddrinfo: %s\n", gai_strerror(err));
		return 5;
	}
	if( result == NULL)
	{
		return 4;
	}
	
	sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (sockfd == -1)
	{
		return 1;
	}
	
	server_addr = (struct sockaddr_in *) result->ai_addr;
	(*server_addr).sin_port = htons(num);
	if (connect(sockfd, result->ai_addr, result->ai_addrlen) == -1)
	{
		return 2;
	}
	
	if (write(sockfd, msg, strlen(msg)) != strlen(msg))
	{
		return 3;
	}
	
	
	close(sockfd);
	return 0;
}

req* read_config(unsigned int node_num)
{
	char buffer[1024];
	char config_name[1024];
	ifstream config_file;
	
	if ( node_num < 10)
	{
		sprintf(config_name, "net0%d", node_num);
	}
	else
	{
		sprintf(config_name, "net%d", node_num);
	}
	
	req* head = (req*) malloc(sizeof(req));
	memset(head, 0x00, sizeof(req));
	
	req* current = head;
	
	config_file.open(config_name, ios::in | ios::binary);
	memset(buffer, 0x00, sizeof(buffer));
	
	config_file >> buffer;
	
	while( buffer[0] != 0)
	{
		current->drop = (int) buffer[0];
		current->time = (int) buffer[1];
		current->next = (req*) malloc(sizeof(req));
		current->next->prev = current;
		current = current->next;
		config_file >> buffer;
	}
	
	return head;
}

void handle( unsigned int client_sock, int net_status)
{
	char buffer[1024];
	char to_addr[18];
	char log_name[9];
	unsigned int node_num;
	ofstream log_file;
	
	memset(buffer, 0x00, sizeof(buffer));
	recv(client_sock, buffer, 1024, 0);
	node_num = buffer[0];
	if (node_num > 45 || node_num == 0)
	{
		send(client_sock, "1", 1, 0); //Invalid Node Number
		return;
	}
	send(client_sock, "0", 1, 0); //respond that node number is valid
	
	if( node_num < 10)
	{
		sprintf(log_name, "log0%d.txt", node_num);
	}
	else
	{
		sprintf(log_name, "log%d.txt", node_num);
	}
	
	log_file.open(log_name, ios::out | ios::binary);
	req* current = read_config(node_num);
	
	
	while (1)
	{
		memset(buffer, 0x00, 1024);
		recv(client_sock, buffer, 1024, 0);
		
		unsigned int t = buffer[0] & 0xff;
		log_file << buffer << endl;
		
		if (t == 255) //Control message
		{
			//all messages are logged. These are only logged
		}
		else if (t > 45 || t == 0) //If "To" is invalid, exit
		{
			send(client_sock, "0", 1, 0); // 0 means no problem, there's never a problem exiting
			break;
		}
		else if (t < 10) //format "To" Hostname
		{
			sprintf(to_addr, "net0%d.utdallas.edu", buffer[0]);
		}
		else
		{
			sprintf(to_addr, "net%d.utdallas.edu", buffer[0]);
		}
	
		int temp = forward(buffer, to_addr);
		send(client_sock, &temp, 4, 0);
	}

	close(client_sock);
	log_file.close();

	return;
}

int main(int argc, char *argv[])
{
	struct sockaddr_in server_addr, client_addr;
	unsigned int server_sock, client_sock, ids;
	int addr_len, port, net_status;

	if (argc != 2)
	{
		printf("\nusage: %s <network status>\n\n", argv[0]);
		exit(-1);
	}
	
	net_status = strtol( argv[1], NULL, 10 );
	port = 4567;

	pthread_attr_t attr;
	pthread_t threads;

	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	 
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
 
	if (listen(server_sock, PEND_CONNECTIONS) != 0)
	{
		printf("Failed to listen to port %d", port);
		return 1;
	}

	pthread_attr_init(&attr);

	pid_t pid;

	while(1)
	{
		client_sock = accept(server_sock, (struct sockaddr *)&client_addr, (socklen_t*) &addr_len);
		pid = fork();

		if (pid > 0)
		{
			handle(client_sock, net_status);
			return 0;
		}

	}


	return 0;
}
