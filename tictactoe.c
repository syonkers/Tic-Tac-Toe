#include "tictactoe.h"
#include "socket.h"

#include <ncurses.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h> 

#define POS0 0x00
#define POS1 0x01
#define POS2 0x02
#define POS3 0x03
#define POS4 0x04
#define POS5 0x05
#define POS6 0x06
#define POS7 0x07
#define POS8 0x08

static int moveList[9] = {9, 9, 9, 9, 9, 9, 9, 9, 9};
static bool amiX;
static bool isMyTurn;
static uint8_t seqNum = 0x00;

uint8_t get_seqNum(){
    return seqNum;
}

void increase_seqNum(){
    seqNum++;
}

void set_amiX(bool XorO){
    amiX = XorO;
}

bool get_amiX(){
    return amiX;
}

void set_isMyTurn(bool YorN){
    isMyTurn = YorN;
}

bool get_isMyTurn(){
    return isMyTurn;
}

//start ncurses with all intial function calls
void startNCurses(){
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);    
}

// draw the game board onto ncurses screen
void drawBoard(){
    drawBorder(0, 28, 0, 19);
    drawBorder(30, 79, 9, 19);
    drawBorder(0, 79, 20, 23);

    int x;
    int y;

    //draw the vertical lines of game board
     for (y = 0; y <= 18; y++){
	mvprintw(y, 9, "|");
	mvprintw(y, 18, "|");
	
    }
    
    //draw horizontal lines of game board
    for(x = 0; x <= 27; x++){
	mvprintw(6, x, "-");
	mvprintw(12, x, "-");
    }
    
    mvprintw(4,32, "Controls:");
    mvprintw(5, 30, "** ~ (tilde) to switch between chat and game");
    mvprintw(6, 30, "** Enter to select/send move or message");

    move(1, 1);

    refresh();
    
}

//draw borders around windows to better define space
void drawBorder(int x1, int x2, int y1, int y2){

    for( int i = y1; i < (y2-1); i++){
	mvaddch(i, x1, '|');
	mvaddch(i, x2-1, '|');
    }
    
    for(int i =  x1 ; i < (x2-1); i++){
	mvaddch(y1 , i, '-');
	mvaddch(y2-1, i, '-');
    }
    
    mvaddch(y1, x1, '+');
    mvaddch(y2-1, x1, '+');
    mvaddch(y1, x2-1, '+');
    mvaddch (y2-1, x2-1,'+');
    
}

//draw an X at the top left xy coordinate
void drawX(int x, int y){
    mvprintw(y, x+2, "_");
    mvprintw(y, x+5, "_");
    mvprintw(y+1, x+1, "(");
    mvprintw(y+1, x+3, "\\");
    mvprintw(y+1, x+4, "/");
    mvprintw(y+1, x+6, ")");
    mvprintw(y+2, x+2, ")");
    mvprintw(y+2, x+5, "(");
    mvprintw(y+3, x+1, "(");
    mvprintw(y+3, x+2, "_");
    mvprintw(y+3, x+3, "/");
    mvprintw(y+3, x+4, "\\");
    mvprintw(y+3, x+5, "_");
    mvprintw(y+3, x+6, ")");
    refresh();
}

//draw an O in ascii at the top left xy coordinate
void drawO(int x, int y){
    mvprintw(y, x+3, "_");
    mvprintw(y, x+4, "_");
    mvprintw(y+1, x+2, "/");
    mvprintw(y+1, x+5, "\\");
    mvprintw(y+2, x+1, "(");
    mvprintw(y+2, x+4, "o");
    mvprintw(y+2, x+6, ")");
    mvprintw(y+3, x+2, "\\");
    mvprintw(y+3, x+3, "_");
    mvprintw(y+3, x+4, "_");
    mvprintw(y+3, x+5, "/");
    refresh();
}

//return the quadrant of the move selected based on x and y coordinates
int getMovePosition(){
    int x, y;

    getyx(stdscr, y, x);
   
    if((y == 1) && (x == 1)){
	return(POS0);
    }else if((y == 1) && (x == 10)){
	return(POS1);
    }else if((y == 1) && (x == 19)){
	return(POS2);
    }else if((y == 7) && (x == 1)){
	return(POS3);
    }else if((y == 7) && (x == 10)){
	return(POS4);
    }else if((y == 7) && (x == 19)){
	return(POS5);
    }else if((y == 13) && (x == 1)){
	return(POS6);
    }else if((y == 13) && (x == 10)){
	return(POS7);
    }else if((y == 13) && (x == 19)){
	return(POS8);
    }else{
	return(0x09);
    }
}

//print the enemy move based on quadrant to xy position
//checks for win condition after placed
void printEnemyMove(uint8_t position){
    int x, y;
    
    if(position == POS0){
	x = 1;
	y = 1;
    }
    else if(position == POS1){
	x = 10;
	y = 1;
    }
    else if(position == POS2){
	x = 19;
	y = 1;
    }
    else if(position == POS3){
	x = 1;
	y = 7;
    }
    else if(position == POS4){
	x = 10;
	y = 7;
    }
    else if(position == POS5){
	x = 19;
	y = 7;
    }
    else if(position == POS6){
	x = 1;
	y = 13;
    }
    else if(position == POS7){
	x = 10;
	y = 13;
    }
    else if(position == POS8){
	x = 19;
	y = 13;
    }

    if(get_amiX()){
	drawO(x, y);
	moveList[position] = 0;
	if(checkWinConditions(0)){
	    mvprintw(1, 40, "You loose");
	}
    }
    else{
	drawX(x, y);
	moveList[position] = 1;
	if(checkWinConditions(1)){
	    mvprintw(1, 40, "You loose");
	}
    }
}

//check win condition based on moveList array
bool checkWinConditions(int player){

    //check columns
    for(int i = 0; i <= 2; i++){
	if((moveList[i] == player)&&(moveList[i+3] == player) && (moveList[i+6] == player)){
	    return true;
	}
    }

    //check rows
    for(int i = 0; i <= 6; i+=3){
	if((moveList[i] == player)&&(moveList[i+1] == player)&&(moveList[i+2] == player)){
	    return true;
	}
    }

    //check diagonals
    if((moveList[0] == player) && (moveList[4] == player) && (moveList[8] == player)){
	return true;
    }
    
    if((moveList[2] == player) && (moveList[4] == player) && (moveList[6] == player)){
	return true;
    }
    return false;
}

//process keyboard input
void processKeyboard(int udpsd){
    static int messagePosition = 0;
    static char message[64];
    static bool typing = false;
    uint8_t buffer[66];
    int positionX;
    int positionY;

    getyx(stdscr,positionY, positionX);

    int input = getch();

    switch(input){
	
    case KEY_UP:
	if(typing == false){
	    if(positionY == 7 || positionY == 13){
		move(positionY = positionY - 6, positionX);
	    }
	}
	break;

    case KEY_DOWN:
	if(typing == false){
	    if(positionY == 1 || positionY == 7){
		move(positionY = positionY + 6, positionX);
	    }
	}
	break;

    case KEY_LEFT:
	if(typing == false){
	    if(positionX == 10 || positionX == 19){
		move(positionY, positionX = positionX - 9);
	    }
	}
	break;

    case KEY_RIGHT:
	if(typing == false){
	    if(positionX == 1 || positionX == 10){
		move(positionY, positionX = positionX + 9);
	    }
	}
	break;

    case KEY_BACKSPACE:
	if((typing == true) && (messagePosition > 0)){
	    message[messagePosition--] = '\0';
	}
	refresh();
	break;
	
    case '`':
	if(get_isMyTurn()){
	    if(typing == false){
		typing = true;
		move(21, 1);
	    }
	    else{
		typing = false;
		move(1,1);
	    }
	}
	else{
	    typing = true;
	    move(21, 1);
	}
	refresh();
	break;

    case '\n':
	if((typing == false) && (get_isMyTurn() == true)){
	    if(amiX){
		drawX(positionX, positionY);
		moveList[getMovePosition()] = 1;
		if(checkWinConditions(1)){
		    mvprintw(1, 40, "You win!!!");
		}
	    }
	    else{
		drawO(positionX, positionY);
		moveList[getMovePosition()] = 0;
		if(checkWinConditions(0)){
		    mvprintw(1, 40, "You win!!!");
		}
	    }
	    uint8_t response[3] = {'G', get_seqNum(), getMovePosition()};
	    sendDatagram(udpsd, response);
	    memset(&response, 0, sizeof(response));
	    move(21, 1);
	    set_isMyTurn(false);
	    typing = true;
	}
	else{
	    buffer[0] = (uint8_t)'M';
	    buffer[1] = get_seqNum();
	    for(int i = 0; i <= messagePosition; i++){
		buffer[i+2] = (uint8_t)message[i];
	    }
	    buffer[messagePosition+3] = (uint8_t)'\0';
	    sendDatagram(udpsd, buffer);
	    memset(&message, 0, sizeof(message));
	    memset(&buffer, 0, sizeof(buffer));
	    messagePosition = 0;
	    move(21, 1);
	}
	break;
    
    
    default:
	if(typing == true){
	    if(messagePosition < 64){
		message[messagePosition++] = (char)input;
		mvprintw(21, 1, "%-*s", messagePosition-1, message);
	    }
	}
	break;
    }
    refresh();
}

