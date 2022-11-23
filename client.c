https://powcoder.com
代写代考加微信 powcoder
Assignment Project Exam Help
Add WeChat powcoder
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAXDATASIZE 50

int main(int argc, char *argv[]) {
    int clientsk, numbytes;
    fd_set livefdset;
    bool waiting_for_user_input = true;

    /* check usage */
    if (argc != 3) {
      fprintf(stderr, "usage : %s <servhost> <servport>\n", argv[0]);
      exit(1);
    }

    /* client create socket */
    if((clientsk = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      fprintf(stderr, "Can't create socket.\n");
      exit(1);
    }

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(struct sockaddr_in));
    sin.sin_family = AF_INET;
    inet_aton(argv[1], &sin.sin_addr);
    sin.sin_port = htons(atoi(argv[2]));

    /* connect to server */
    if(connect(clientsk, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
       fprintf(stderr, "Can't connect to server.\n");
       close(clientsk);
       exit(1);
    }

    while(1) {
        char buf[MAXDATASIZE];

        FD_ZERO(&livefdset);
        /* add standard input to livefdset */
        FD_SET(0, &livefdset);
        FD_SET(clientsk, &livefdset);
	
	if(waiting_for_user_input) {
	    fprintf(stdout, "msg: ");
	    fflush(stdout);
	}

	if(select(clientsk + 1, &livefdset, NULL, NULL, NULL) == -1) {
	    fprintf(stderr, "Can't select.\n");
	    exit(1);
	}		


	/* input from socket */
	if(FD_ISSET(clientsk, &livefdset)) {
	    if ((numbytes = recv(clientsk, buf, MAXDATASIZE-1, 0)) == -1) {
                fprintf(stderr, "recv error.\n");
		exit(1);
    	    } 
	    if (numbytes > 0) {
	        buf[numbytes] = '\0';
	        fprintf(stdout, "client: received '%s'\n", buf);
		close(clientsk);
		exit(0);
	    }
	}
	/* input from keyboard */
	if(FD_ISSET(0, &livefdset)) {
	    if (!fgets(buf, MAXDATASIZE, stdin))
		exit(1);
	    send(clientsk, buf, strlen(buf)-1, 0);
	    waiting_for_user_input = false;
	}
    }
    return 0;
}
