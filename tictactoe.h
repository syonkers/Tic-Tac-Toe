#include <stdbool.h>
#include <stdint.h>

#ifndef tictactoe_c
#define tictactoe_c

void increase_seqNum();

uint8_t get_seqNum();

void set_amiX(bool XorO);

bool get_amiX();

void set_isMyTurn(bool YorN);

bool get_isMyTurn();

void startNCurses();

void drawBoard();

void drawBorder(int x1, int x2, int y1, int y2);

void drawX(int x, int y);

void drawO(int x, int y);

int getMovePosition();

void printEnemyMove(uint8_t position);

bool checkWinConditions(int player);

void processKeyboard(int udpsd);

#endif
