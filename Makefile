CC=gcc
CFLAGS=-Wall -Werror

all:
	$(CC) $(CFLAGS) main.c -o main -lSDL
clean:
	rm -rf main
