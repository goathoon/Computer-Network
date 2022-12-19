#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#define SERVER_TCP_PORT 13665
#define BUFLEN 300
#define CLNT_MAX 5

void read_childproc(int sig);
int prc_cnt = 5;
int main(int argc, char **argv)
{
    int sd, new_sd,sock_tok, client_len, port;
    struct sockaddr_in server, client;
    char buf[BUFLEN];
    pid_t pid;
    struct sigaction act;
    int clnt_cnt = 0;    
    int done_clnt = 0;
    int str_len, state,status;

    switch (argc)
    {
    case 1:
        port = SERVER_TCP_PORT;
        break;
    case 2:
        port = atoi(argv[1]);
        break;
    default:
        fprintf(stderr, "Usage: %s [port]\n", argv[0]);
        exit(1);
    }

    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHLD,&act,0);
    if(state==-1){
    	printf("sigaction set fault\n");
    }

    /* Create a stream socket */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Can't create a socket\n");
        exit(1);
    }
    /* Bind an address to the socket */
    bzero((char *)&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        fprintf(stderr, "Can't bind name to socket\n");
        exit(1);
    }
    /* queue up to 5 connect requests */
    if(listen(sd, 5)==-1){
    	fprintf(stderr,"Listen error");
    }
    
    while (1)
    {	

	client_len = sizeof(client);
        new_sd = accept(sd, (struct sockaddr *)&client, &client_len);
        //have same socket  in paretn and child, if forks.
	if(new_sd ==-1)
	{
		puts("no socket available");
		close(new_sd);
		continue;
	}
	else{
	    puts("new client is connected..");
	}    
	pid = fork();
	if(pid ==-1){
		close(new_sd);
		printf("no fork\n");
		continue;
	}
	if(pid ==0) //child process
	{
	
		puts("child?");
		close(sd);
		memset(buf,0x00,BUFLEN);
		while((str_len = read(new_sd,buf,BUFLEN))!=0){
			write(new_sd,buf,str_len);
		}
		printf("here is coming : %s\n",buf);
		close(new_sd);
		puts("clients disconnected..");

		//make a new socket to communincatie me(client)
		bzero((char *)&server, sizeof(struct sockaddr_in));
   		server.sin_family = AF_INET;
    		server.sin_port = htons(13666);
    		server.sin_addr.s_addr = htonl(INADDR_ANY);
		if ((sock_tok = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    		{
        		fprintf(stderr, "Can't create a token socket\n");
        		exit(1);
    		}
		if (connect(sock_tok, (struct sockaddr *)&server,sizeof(server)) == -1)
    		{
       	 		fprintf(stderr, "Can't connect with token socket\n");
        		exit(1);
    		}
		write(sock_tok,buf,BUFLEN);
		return 0;
	}
	else{
		close(new_sd);
	}
      
    }
    close(sd);
    return (0);
}

void read_childproc(int sig){
	pid_t pid;
	int status;
	pid = waitpid(-1,&status,WNOHANG);
	printf("kill process : %d\n",pid);
}
