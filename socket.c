
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>


int createUDPServer(char *port){
    struct addrinfo hints;
    struct addrinfo *res, *iter;
    int sock;
    int code;
    int option = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE | AI_V4MAPPED;
    hints.ai_protocol = 0;

    code = getaddrinfo(NULL, port, &hints, &res);
    if(code != 0){

	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(code));
	exit(1);
    }

    for(iter = res; iter; iter = iter->ai_next){

	sock = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
	if(sock < 0){
	    perror("socket: ");
	    continue;
	}
	
	if( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0){
	    close(sock);
	    sock = -1;
	    continue;
	}

	if( bind( sock, iter->ai_addr, iter->ai_addrlen) < 0){
	    perror("Bind: ");
	    close(sock);
	    sock = -1;
	    continue;
	}
	
	break;
    }

    freeaddrinfo(res);
    if(sock == -1){
	fprintf(stderr, "Unable to create UDP socket\n");
	exit(1);
    }
    
    return sock;
}

int createUDPClient(uint8_t* buffer, char* host, char* port){
    struct addrinfo hints, *res, *iter;
    int code;
    int sendSock;
    int send_len;
    int option = 1;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_V4MAPPED;
    hints.ai_protocol = 0;

    code = getaddrinfo(host, port, &hints, &res);
    if (code != 0){
	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(code));
	exit(1);
    }

    for (iter = res; iter; iter = iter->ai_next){
	if ((sendSock = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol)) < 0){
	    perror("socket: ");
	    continue;
	}
	if ( setsockopt( sendSock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0){
	    close(sendSock);
	    sendSock = -1;
	    continue;
	}
	if( connect (sendSock, iter->ai_addr, iter->ai_addrlen)<0){
	    perror("Connect:");
	    continue;
	}
	break;
    }
    
    freeaddrinfo(res);

    if (sendSock == -1){
	fprintf(stderr, "Unable to create UDP socket\n");
	endwin();
	exit(1);
    }
    
    if ((send_len = sendto(sendSock, buffer, sizeof(buffer), 0,iter->ai_addr, iter->ai_addrlen)) < 0){
	perror("sendto");
	endwin();
	exit(1);
    }
    
    return sendSock;

}

void sendDatagram(int udpsd, uint8_t* buffer){
    int code;
    code = send(udpsd, buffer, sizeof(buffer), 0);
    if (code < 0){
	code = errno;
	printf("Failed to send datagram, error code: (%d)\n", code);
	perror("Additional  info: ");
	close(udpsd);
	endwin();
	exit(1);
    }
}


