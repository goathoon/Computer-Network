#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <errno.h>
#define SERVER_TCP_PORT 50000
#define BUFLEN 256
#define OUT 1
#define IN 0
int main(int argc, char **argv)
{
    int sd, new_sd,clnt_sd,len_clnt,port,state;
    struct hostent *hp;
    struct sockaddr_in server,client;
    char *host, buf[BUFLEN];
    pid_t pid;
    int status = 0;
    switch (argc)
    {
    case 2:
        host = argv[1];
        port = SERVER_TCP_PORT;
        break;
    case 3:
        host = argv[1];
        port = atoi(argv[2]);
        break;
    default:
        fprintf(stderr, "Usage: %s host [port]\n", argv[0]);
        exit(1);
    }
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Can't create a socket\n");
        exit(1);
    }
    bzero((char *)&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if ((hp = gethostbyname(host)) == NULL)
    {
        fprintf(stderr, "Can't get server's address\n");
        exit(1);
    }
    bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);
    /* Connecting to the server */
    if (connect(sd, (struct sockaddr *)&server,
                sizeof(server)) == -1)
    {
        fprintf(stderr, "Can't connect\n");
        exit(1);
    }
	    
    memset(buf,0x00,BUFLEN);
    read(sd,buf,BUFLEN);
    printf("%s",buf);
    memset(buf,0x00,BUFLEN);
    read(sd,buf,BUFLEN);
    printf("%s",buf);
    memset(buf,0x00,BUFLEN);
    fgets(buf,BUFLEN,stdin);
    write(sd,buf,11);

    memset(buf,0x00,BUFLEN);
    read(sd,buf,BUFLEN);
    printf("%s",buf);
    memset(buf,0x00,BUFLEN);
    read(sd,buf,BUFLEN);
    printf("%s",buf);
    memset(buf,0x00,BUFLEN);
    fgets(buf,BUFLEN,stdin);
    write(sd,buf,14);

    memset(buf,0x00,BUFLEN);
    read(sd,buf,BUFLEN);
    printf("%s",buf);
    memset(buf,0x00,BUFLEN);
    read(sd,buf,BUFLEN);
    printf("%s",buf);
    memset(buf,0x00,BUFLEN);
    fgets(buf,BUFLEN,stdin); 
    write(sd,buf,6);

    memset(buf,0x00,BUFLEN);
    read(sd,buf,BUFLEN);
    printf("%s",buf);
    memset(buf,0x00,BUFLEN);
    read(sd,buf,BUFLEN);
    printf("%s",buf);
    memset(buf,0x00,BUFLEN);
    fgets(buf,BUFLEN,stdin);
    write(sd,buf,2);

    memset(buf,0x00,BUFLEN);
    read(sd,buf,BUFLEN);
    printf("%s",buf);
    memset(buf,0x00,BUFLEN);
    read(sd,buf,BUFLEN);
    printf("%s",buf);
    memset(buf,0x00,BUFLEN);
    fgets(buf,BUFLEN,stdin);
    write(sd,buf,2);

    memset(buf,0x00,BUFLEN);
    read(sd,buf,BUFLEN);
    printf("%s",buf);
    memset(buf,0x00,BUFLEN);
    read(sd,buf,BUFLEN);
    printf("%s",buf);
    memset(buf,0x00,BUFLEN);
    fgets(buf,BUFLEN,stdin);
    write(sd,buf,3);

    memset(buf,0x00,BUFLEN);
    read(sd,buf,BUFLEN);
    //printf("%s",buf);
    
    memset(buf,0x00,BUFLEN);
    /*read(sd,buf,BUFLEN);
    printf("%s",buf);
    memset(buf,0x00,BUFLEN);
    fgets(buf,BUFLEN,stdin);*/
	
    int fd[2];
    state = pipe(fd);
    if(state==-1){
    	puts("PIPE FAILED");
	exit(1);
    } 
    int clnt_done = 0;
    char lastbuf[BUFLEN];
    memset(lastbuf,0,BUFLEN);
    while(1){
   	 pid = fork();
    	 if(pid ==0){//at child process, make a server to receive a token number from ipv4 server
		//printf("child\n");
        	new_sd = socket(AF_INET,SOCK_STREAM,0);
		memset(&server,'0',sizeof(server));
		memset(buf,0X00,BUFLEN);
	
		server.sin_family = AF_INET;
		server.sin_addr.s_addr=htonl(INADDR_ANY);
		server.sin_port = htons(13666);
		if(bind(new_sd,(struct sockaddr*)&server,sizeof(server))==-1){
			fprintf(stderr,"BIND FAILED\n");
			return -1;
		}
		
		if(listen(new_sd,5)==-1){
			fprintf(stderr,"LISTEN FAILED\n");
			return -1;
		}
		//printf("listen success\n");	
		len_clnt = sizeof(client);
		clnt_sd = accept(new_sd,(struct sockaddr*)&client,&len_clnt);
        	if(clnt_sd == -1){
			fprintf(stderr,"accept failed\n");
			exit(1);
		}
		//printf("Connection success! \n");

		memset(buf,0x00,BUFLEN);
		read(clnt_sd,buf,BUFLEN);
		//printf("%s\n",buf);
		write(fd[OUT],buf,BUFLEN);
		exit(0);
			
	        }
	else if(pid>0){
		pid_t waitPid;
		memset(buf,0x00,BUFLEN);
		//printf("parent\n");
		waitPid = wait(&status);
		if(waitPid ==-1){
			printf("error num : %d\n",errno);
			perror("wait func eror");
		}
        	else{
			if(WIFEXITED(status)){
				//printf("wait: normal exit %d\n",WEXITSTATUS(status));
			}
			else if(WIFSIGNALED(status)){
				//printf("wait: abnormal exit %d\n",WTERMSIG(status));
			}
	   	}
		//printf("parent procss exit %d %d\n",waitPid, WTERMSIG(status));
		memset(buf,0x00,BUFLEN);
  		buf[strlen(buf)-1]=0;
		memset(buf,0x00,BUFLEN);
		read(fd[IN],buf,BUFLEN);

		//printf("buf length is %ld\n",strlen(lastbuf));
		//write(sd,buf,BUFLEN);
		buf[28] = ',';
		buf[29] = 0;
		strcat(lastbuf,buf);
		clnt_done ++;
		if(clnt_done ==5){
			lastbuf[144] ='\n';
    			break;
			
		}
		//printf("%s\n",lastbuf);

		continue;
	}
   	 else{
    		fprintf(stderr,"fork err\n");
    	}	
 	//printf("%s",lastbuf);
	//write(sd,lastbuf,144);
    	//close(sd);
    	//read(fd[IN],buf,BUFLEN);
    	//printf("here is coming ---\n");

    	return (0);
    }
    
printf("%s",lastbuf);
memset(buf,0X00,BUFLEN);
write(sd,lastbuf,145);
read(sd,buf,BUFLEN);
}
