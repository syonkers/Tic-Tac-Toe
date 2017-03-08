#include <stdint.h>

#ifndef socket_c
#define socket_c


int createUDPServer(char *port);

int createUDPClient(uint8_t *buffer, char *host, char *port);

void sendDatagram(int sock, uint8_t *buffer);

#endif
