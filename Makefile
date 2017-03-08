CC = gcc
CFLAGS = -D_POSIX_C_SOURCE=200809L -Wall -std=c99
LDLIBS = -lncurses

ttt: main.o socket.o parse.o tictactoe.o
	$(CC) $(LDFLAGS) main.o socket.o parse.o tictactoe.o $(LDLIBS) -o ttt

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

socket.o: socket.c socket.h
	$(CC) $(CFLAGS) -c socket.c socket.h

parse.o: parse.c parse.h
	$(CC) $(CFLAGS) -c parse.c parse.h

tictactoe.o: tictactoe.c tictactoe.h
	$(CC) $(CFLAGS) -c tictactoe.c tictactoe.h

clean:
	$(RM) ttt main.o socket.o parse.o socket.h.gch parse.h.gch tictactoe.o tictactoe.h.gch
