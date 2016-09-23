CC=gcc
CFLAGS=-Wall -Werror

all:
	$(CC) $(CFLAGS) main.c -o main -lSDL -lSDL_image
clean:
	rm -rf main
