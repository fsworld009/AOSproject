#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#define LSERVER_BUFFER_SIZE 1024
#define LSERVER_PORT 6789

enum ROLE{NODE, SWITCH} ;

int get_netid(){
    int netid=0;
    FILE *cmd=popen("uname -a | cut -d \" \" -f2","r"); //execute Linux command to get "netXX.utdallas.edu"
    char result[50];
    fgets(result, sizeof(result), cmd);
    char num[3];
    num[0]=result[3];
    num[1]=result[4];
    num[2]='\0';        //num="XX", XX is the net id
    netid = atoi(num);  //convert the num string into int
    pclose(cmd);
    return netid;
}

ROLE get_role(int netid){
    ROLE role;
    /*
        open config file, findout my role
    */
    return role;
}

int main(int argc, char *argv[])
{
	printf("Start listening server\n");
    int netid = get_netid();
    printf("My netid is %d\n",netid);
    
    ROLE role = get_role(netid);
    
    struct sockaddr_in server_addr, client_addr;
	unsigned int server_sock, client_sock, ids;
	int addr_len, port, net_status;

	/*if (argc != 2)
	{
		printf("\nusage: %s <network status>\n\n", argv[0]);
		exit(-1);
	}*/
	
	//net_status = strtol( argv[1], NULL, 10 );
	port = LSERVER_PORT;

	//pthread_attr_t attr;
	//pthread_t threads;

    
    int option=1;
    setsockopt(server_sock,SOL_SOCKET,SO_REUSEADDR,(char*)&option,sizeof(option));
    
	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	 
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
 
	if (listen(server_sock, 100) != 0)
	{
		printf("Failed to listen to port %d", port);
		return 1;
	}else{
        printf("ListenServer:: start to listen\n");
    }

	//pthread_attr_init(&attr);

	pid_t pid;

	while(1)
	{
		client_sock = accept(server_sock, (struct sockaddr *)&client_addr, (socklen_t*) &addr_len);
		pid = fork();

		if (pid > 0)
		{
            char buff[LSERVER_BUFFER_SIZE];
            bzero(buff,LSERVER_BUFFER_SIZE);
			recv(client_sock, buff, LSERVER_BUFFER_SIZE, 0);
            printf("RECV %s\n",buff);
            
            /*case get '0'
            if(strcpy(buff,"0")==0){
                if(role==NODE){
                    get node number;
                    system("./node.out")
                }else{
                    system("./vswitch.out");
                }
            }
            
            
            */
            
			return 0;
		}

	}


	return 0;
}
