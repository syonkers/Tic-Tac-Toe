#include "socket.h"
#include "tictactoe.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <ncurses.h>

#define T 'T'
#define A 'A'
#define P 'P'
#define E 'E'
#define G 'G'
#define M 'M'
#define Q 'Q'


void handleGDatagram(int udpsd, uint8_t *buffer){
    int x, y;
    getyx(stdscr, y, x);  
    printEnemyMove(buffer[2]);
    move(y, x);
    refresh();
}

void handleMDatagram(int udpsd, uint8_t *buffer){
    char message[100];
    int i = 2;
    int x, y;
    getyx(stdscr, y, x);
    while((char)buffer[i] != '\0'){
	message[i-2] = (char)buffer[i];
	i++;
    }
    mvprintw(17,32, "%s", message);
    memset(&message, 0, sizeof(message));
    move(y, x);
    refresh();
}

void processDatagram(int udpsd){
    int recv_len;
    int code;
    uint8_t* buffer = malloc(66 * sizeof(uint8_t));
    uint8_t response[3];
    uint8_t type;
    bool sendBack = false;

    recv_len = recv(udpsd, buffer, sizeof(buffer), 0);
    if(recv_len > 0){
	type = buffer[0];
	switch(type){
	case T:
	    response[0] = (uint8_t)'Q';
	    sendBack = true;
	    break;
	case A:
	    increase_seqNum();
	    break;
	case P:
	    if(buffer[2] == 0x00){
		response[0] = (uint8_t)'P';
		response[1] = buffer[1];
		response[2] = 0x01;
		sendBack = true;
	    }
	    else{
		increase_seqNum();
	    }
	    break;
	case E:
	    if(buffer[2] == 0x01){
		mvprintw(7, 31, "Error: Illegal move");
	    }
	    else{
		mvprintw(7, 31, "Error: Not your turn");
	    }
	    break;
	case G:
	    if(get_isMyTurn()){
		response[0] = (uint8_t)'E';
		response[1] = buffer[1];
		response[2] = 0x02;
		sendBack = true;
	    }
	    else{
		handleGDatagram(udpsd, buffer);
		set_isMyTurn(true);
	    }
	    break;
	case M:
	    handleMDatagram(udpsd, buffer);
	    break;
	case Q:
	    mvprintw(7, 31, "Player has quit the game, closing in 3 seconds");
	    sleep(3);
	    endwin();
	    exit(0);
	    break;
	}
    }
    if(sendBack == true){
	code = send(udpsd, response, sizeof(response), 0);
	if ( code < 0){
	    perror("send");
	}
	memset(&response[0], 0, sizeof(response));
    }
    free(buffer);
    //memset(&buffer[0], 0, sizeof(buffer));
}

bool checkGameStart(int udpsd, bool connecting){
    struct sockaddr_storage clientAddr;
    socklen_t clientAddrLength = sizeof(clientAddr);
    int recv_len;
    uint8_t buffer[80];
    uint8_t type;
    
    recv_len = recvfrom (udpsd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddr, &clientAddrLength);
    if(recv_len > 0){
	type = buffer[0];
	//if we were listening for an incoming request, we want to receive a 'T' packet
	if(connecting == false){ 
	    if(type == T){
		connect(udpsd, (struct sockaddr *)&clientAddr, clientAddrLength);
		if(buffer[2] == 0x01){
		    set_amiX(true);
		    set_isMyTurn(true);
		    mvprintw(8, 31, "You are X");
		    move(1,1);
		    refresh();
		}
		else{
		    set_amiX(false);
		    set_isMyTurn(false);
		    mvprintw(8, 31, "You are O");
		    move(1,1);
		    refresh();
		}
		return true;
	    }
	    else{
		return false;
	    }
	}
	//if we were the one to send the intial request, we want an acknowldgement back
	else{
	    if(type == A){
		connect(udpsd, (struct sockaddr *)&clientAddr, clientAddrLength);
		return true;
	    }
	    else{
		return false;
	    }
	}
    }
    return false;
}
