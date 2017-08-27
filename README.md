Webcam Streaming with V4L2 and SDL
==================================

This code basically handles a webcam, activates the stream mode and
renders it on a SDL Window.

Most part of the code was based on some other implementations like:
https://github.com/kryali/SmileTime/blob/master/SDL-capture-example.cpp
http://www.cs.fsu.edu/~baker/devices/projects/pendawchi/src/capture.c
https://jwhsmith.net/2014/12/capturing-a-webcam-stream-using-v4l2/

In these cases, the developer converted YUYV (grabbed from webcam) to RGB, to be able to draw it
in an SDL_Surface. So, I didn't wanted to learn such algorithm, I used SDL_Overlay, which
is a "window" created over an SDL_Surface, but with the advantage of using YUYV
directly, without converting formats.

For MJPEG, SDL loads the picture from memory, and then blits it to surface. Easier in this
case.

Dependencies:
SDL-devel
SDL-image-devel

Notes:
* This code just implements YUVY and MJPEG, but better cameras should have another formats
* It's a very basic support, just an example of how to do it

References:
http://sdl.beuc.net/sdl.wiki/SDL_Overlay
https://en.wikipedia.org/wiki/YUV

Build
=====

To test this code, need the following dependencies:
* meson (build system)
* SDL2-devel
* SDL2_image-devel

If you want to try this app using flatpak, you can follow the steps below:
* Install flatpak
* Execute make (it will install this application locally)
* Execute "flatpak run org.test.V4lWebcam" to run the app
