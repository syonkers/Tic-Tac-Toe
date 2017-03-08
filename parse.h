#include <stdbool.h>

#ifndef parse_c
#define parse_c

void processDatagram(int udpsd);

bool checkGameStart(int udpsd, bool connecting);

#endif
