CC=gcc
CFLAGS=-Wall -Werror

all:
	$(CC) $(CFLAGS) main.c -o main
	$(CC) -Wall ex.c -o ex -lSDL
	$(CC) $(CFLAGS) hello_xlib.c -L/usr/X11R6/lib -lX11 -o hello_xlib
