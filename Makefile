CC=gcc
CFLAGS=-Wall -Werror
LDFLAGS=-lSDL -lSDL_image

all:
	$(CC) $(CFLAGS) v4l_webcam.c -o v4l_webcam $(LDFLAGS)
clean:
	rm -rf v4l_webcam
