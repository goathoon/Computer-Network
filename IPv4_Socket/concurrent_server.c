#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>

#define SERVER_TCP_PORT 13665
#define BUFLEN 300
#define CLNT_MAX 5

void read_childproc(int sig);

int main(int argc, char **argv)
{
    int sd, new_sd, sock_tok, client_len, port;
    struct sockaddr_in server, client;
    char buf[BUFLEN];
    pid_t pid;
    struct sigaction act;
    int clnt_cnt = 0;
    int done_clnt = 0;
    int str_len, state, status;

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
    state = sigaction(SIGCHLD, &act, 0);
    if (state == -1)
    {
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

    /* Queue up to 5 connect requests */
    if (listen(sd, 5) == -1)
    {
        fprintf(stderr, "Listen error\n");
    }

    for (int i = 0; i < CLNT_MAX; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            // Child process
            while (1)
            {
                client_len = sizeof(client);
                new_sd = accept(sd, (struct sockaddr *)&client, &client_len);
                if (new_sd == -1)
                {
                    puts("No socket available");
                    continue;
                }
                else
                {
                    puts("New client connected");
                }

                memset(buf, 0x00, BUFLEN);
                while ((str_len = read(new_sd, buf, BUFLEN)) != 0)
                {
                    write(new_sd, buf, str_len);
                }
                printf("Here is coming: %s\n", buf);
                close(new_sd);
                puts("Client disconnected");

                // Make a new socket to communicate with the server
                bzero((char *)&server, sizeof(struct sockaddr_in));
                server.sin_family = AF_INET;
                server.sin_port = htons(13666);
                server.sin_addr.s_addr = htonl(INADDR_ANY);
                if ((sock_tok = socket(AF_INET, SOCK_STREAM, 0)) == -1)
                {
                    fprintf(stderr, "Can't create a token socket\n");
                    exit(1);
                }
                if (connect(sock_tok, (struct sockaddr *)&server, sizeof(server)) == -1)
                {
                    fprintf(stderr, "Can't connect with token socket\n");
                    exit(1);
                }
                write(sock_tok, buf, BUFLEN);
                close(sock_tok);
            }
            close(sd);
            return 0;
        }
        else if (pid < 0)
        {
            printf("Fork failed\n");
            exit(1);
        }
    }

    close(sd);
    while (1)
    {
        pause(); // Wait for terminated child process
    }

    return 0;
}

void read_childproc(int sig)
{
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        printf("Killed process: %d\n", pid);
    }
}
