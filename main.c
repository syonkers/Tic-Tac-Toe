/****************************
/Shaun Yonkers
/1650692
/CMPT 361: Assignment 2
/Tic Tac Toe Game
*****************************/
#include "socket.h"
#include "parse.h"
#include "tictactoe.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/wait.h>

#include <netdb.h>
#include <getopt.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <ncurses.h>

#define DEFAULT_PORT "5674"


int enterGameLoop(char *host, char *port, bool connectMode){
    int udpsd;
    bool gameStart = false;
    
    startNCurses();
    drawBoard();

    if(connectMode == false){
	udpsd = createUDPServer(port);
    }
    
    if((connectMode == true)&&(gameStart == false)){
	uint8_t x = rand() % 2;
	if(x == 0x01){
	    mvprintw(8, 31, "You are O");
	    set_amiX(false);
	    set_isMyTurn(false);
	}
	else{
	    mvprintw(8, 31, "You are X");
	    set_amiX(true);
	    set_isMyTurn(true);
	}
	move(1,1);
	refresh();
	uint8_t currentDatagram[3] = {(uint8_t)'T', 0x00, x};
	udpsd = createUDPClient(currentDatagram, host, port);
	memset(&currentDatagram[0],0,sizeof(currentDatagram));
	
    }
    
    while(true){
	time_t currentTime;
	time_t PingTimer;
	int rdy;
	struct timeval timeout;
	fd_set rfds;
	
	FD_ZERO(&rfds);
	FD_SET(udpsd, &rfds);
	FD_SET(0, &rfds);

	timeout.tv_sec = 0;
	timeout.tv_usec = 100;
	
	rdy = select(udpsd + 1, &rfds, NULL, NULL, &timeout);
	if(rdy == -1){
	    if(errno == EINTR){
		continue;
	    }
	    perror("select");
	    exit(EXIT_FAILURE);
	}
	else if(rdy == 0){
	    if(gameStart == true){
		currentTime = time(NULL);
		if (currentTime >= PingTimer){
		    uint8_t currentDatagram[3] = {'P', get_seqNum(), 0x00};
		    sendDatagram(udpsd, currentDatagram);
		    memset(&currentDatagram[0], 0, sizeof(currentDatagram));
		    increase_seqNum();
		    PingTimer = time(NULL) + 5;
		}
	    }
	}
	else{
	    if(FD_ISSET(udpsd, &rfds)){
		if(gameStart == false){
		    gameStart = checkGameStart(udpsd, connectMode);
		    if(gameStart == true){
			PingTimer = time(NULL) + 5;
			if(connectMode == false){
			    uint8_t currentDatagram[3] = {(uint8_t)'A', 0x00, 0x00};
			    sendDatagram(udpsd,currentDatagram);
			    memset(&currentDatagram[0],0,sizeof(currentDatagram)); 
			}
		    }
		}
		else{
		    processDatagram(udpsd);
		}
	    }
	    if(FD_ISSET(0, &rfds)){
		processKeyboard(udpsd);
	    }
	}
    }
    return(0);
}


int main(int argc, char** argv){
    int opt;
    int port;
    char *host = "";
    char *portFlag = "-1";
    extern char *optarg;
    bool listening = false;
    bool connect = false;
    
    while((opt = getopt(argc, argv, ":hlc:p:")) != -1){
	switch(opt){
	case 'h':
	    printf("****Usage Information****\n");
	    printf("-h display the usage information");
	    printf("-c <host> connect to <host>");
	    printf("-l run in listen mode");
	    printf("-p specify a port number");
	    return(0);
	case 'p':
	    portFlag = optarg;
	    port = (long)portFlag;
	    break;
	case 'l':
	    listening = true;
	    break;
	case 'c':
	    connect = true;
	    host = optarg;
	    break;
	}
    }
    //if the user select both to connect and listen we send an error and exit 
    if (listening == true && connect == true){
	fputs("Error: Unable to both connect and listen simultaneously, please select either the -l OR -c option\n", stderr);
	exit(0);
    }
    //if the user has selected neither of the two option, we send and error and exit
    if(listening == false && connect == false){
	fputs("Error: User needs to select an option to play game, for more info please add the -h option for help\n", stderr);
	exit(0);
    }
    else{
	if(port > 0){
	    enterGameLoop(host, portFlag, connect);
	}
	else{
	    enterGameLoop(host, DEFAULT_PORT, connect);
	}
    }
    return(0);
}



