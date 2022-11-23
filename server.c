https://powcoder.com
代写代考加微信 powcoder
Assignment Project Exam Help
Add WeChat powcoder
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>

#define MAXDATASIZE 50

int serversk;

/* handler function to handle SIGINT */
void sig_handler(int signo)
{
  if (signo == SIGINT)
    printf("received SIGINT, terminating server...\n");
    close(serversk);
    exit(0);
}

int main(int argc, char *argv[]) {
    struct sockaddr_in sin;

    /* check usage */
    if (argc != 1) {
        fprintf(stderr, "usage : %s\n", argv[0]);
        exit(1);
    }

    /* create TCP socket */
    if((serversk = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      fprintf(stderr, "Can't create socket.\n");
      exit(1);
    }

    /* bind socket to some port using bind() */
    /* specify port 5555 */
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(5555);

    if(bind(serversk, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
        fprintf(stderr, "Can't bind socket.\n");
        close(serversk);
        exit(1);
    }

    /* ready to receive connections */
    if (listen(serversk, 5) == -1) {
        fprintf(stderr, "listen error.\n");
        exit(1);
    }

    fprintf(stdout, "admin: started server at '%hu'\n", 5555);

    /* invoke sig_handler when user type CTRL-C */
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
	fprintf(stderr, "cannot catch SIGINT\n");
        exit(1);	
    }

    while(1) {
	int newfd;
	struct sockaddr_in client;
	socklen_t len = sizeof(struct sockaddr);	
	if ((newfd = accept(serversk,(struct sockaddr*)&client, &len)) == -1) {
	    fprintf(stderr, "Can't accept new connection.\n");
	    close(serversk);
	    exit(1);
	}
	fprintf(stdout, "connection request from [%s:%hu]\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

	/* fork a child process to handle this new connection */
	if (!fork()) { 
	    char buf[MAXDATASIZE];
	    int numbytes, i;
	    /* chile process does not need listener socket */
            close(serversk); 
	    /* receive message from client */
	    if ((numbytes = recv(newfd, buf, MAXDATASIZE-1, 0)) == -1) {
                fprintf(stderr, "recv error.\n");
                exit(1);
            }
	    buf[numbytes] = '\0';

	    fprintf(stdout, "server: received '%s'\n", buf);   
	    for (i = 0; i < numbytes; i++) {
		buf[i] = toupper(buf[i]);
	    }
	    send(newfd, buf, strlen(buf), 0);
            close(newfd);
            exit(0);
        }
	/* parent process does not need this */
        close(newfd);  
    }
    return 0;
}
