/*
 *  V4L2 video capture example
 *
 *  This program can be used and distributed without restrictions.
 */
#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

struct buffer {
        void *                  start;
        size_t                  length;
};

static char *           dev_name        = "/dev/video0";
static int              fd              = -1;
struct buffer *         buffers         = NULL;
static unsigned int     n_buffers       = 0;
static int video_width=640;
static int video_height=480;
static int video_depth=16;
SDL_Surface *surface;		/* the main window surface */

static void errno_exit(const char *s)
{
        fprintf (stderr, "%s error %d, %s\n",
                 s, errno, strerror (errno));

        exit (EXIT_FAILURE);
}

static int xioctl(int fd, int request, void *arg)
{
        int r;
        do {
		r = ioctl (fd, request, arg);
        } while (-1 == r && EINTR == errno);

        return r;
}

int update_frame(const void * p)
{
	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);

	memcpy(surface->pixels, (unsigned char *)p, video_width * video_height * (video_depth / 8));

	SDL_UnlockSurface(surface);
	SDL_Flip(surface);	/* should wait for vsync */

	return 0;
}

static int read_frame(void)
{
	struct v4l2_buffer buf;

	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if (xioctl (fd, VIDIOC_DQBUF, &buf) == -1) {
		printf("xioctl dqbuf is wrong !!!\n");
		switch (errno) {
			case EAGAIN:
				return 0;
			case EIO:
				/* Could ignore EIO, see spec. */
				/* fall through */
			default:
				errno_exit ("VIDIOC_DQBUF");
		}
	}

	assert (buf.index < n_buffers);
	printf("index = %d\n",buf.index);
	update_frame(buffers[buf.index].start);

	if (xioctl (fd, VIDIOC_QBUF, &buf) == -1)
		errno_exit ("VIDIOC_QBUF");

	return 1;
}

static void mainloop(void)
{
	unsigned long count = 100;

	while (count-- > 0) {
		for (;;) {
			fd_set fds;
			struct timeval tv;
			int r;

			FD_ZERO (&fds);
			FD_SET (fd, &fds);

			/* Timeout. */
			tv.tv_sec = 1;
			tv.tv_usec = 0;

			r = select (fd + 1, &fds, NULL, NULL, &tv);
			switch(r) {
			case -1:
				if (EINTR == errno)
					continue;
				errno_exit ("select");
				break;
			case 0:
				fprintf (stderr, "select timeout\n");
				exit (EXIT_FAILURE);
				break;
			}

			if (read_frame())
				break;
		}
	}
}

static void stop_capturing(void)
{
        enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl (fd, VIDIOC_STREAMOFF, &type))
		errno_exit ("VIDIOC_STREAMOFF");
}

static void start_capturing(void)
{
        unsigned int i;
        enum v4l2_buf_type type;

	for (i = 0; i < n_buffers; ++i) {
		struct v4l2_buffer buf;

		memset(&buf, 0, sizeof(buf));
		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = i;

		if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
			errno_exit ("VIDIOC_QBUF ... !!!");
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl (fd, VIDIOC_STREAMON, &type))
		errno_exit ("VIDIOC_STREAMON");
}

static void uninit_device(void)
{
        unsigned int i;
	for (i = 0; i < n_buffers; ++i)
		if (-1 == munmap (buffers[i].start, buffers[i].length))
			errno_exit ("munmap");

	free (buffers);
}
int init_view()
{
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("Unable to initialize SDL\n");
		return -1;
	}
	if ((surface = SDL_SetVideoMode(video_width, video_height, video_depth, SDL_HWSURFACE)) == NULL) {
		printf("Unable to obtain video mode %dx%dx%d\n", video_width, video_height, video_depth);
		return -1;
	}

	return 0;
}

void close_view()
{
	if (surface)
		SDL_FreeSurface(surface);

	surface = (SDL_Surface *)NULL;
	if (SDL_WasInit(SDL_INIT_VIDEO)) 
		SDL_Quit();
}


static void init_mmap(void)
{
	struct v4l2_requestbuffers req;

	memset(&req, 0, sizeof(req));
	req.count  = 4;
	req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (xioctl (fd, VIDIOC_REQBUFS, &req) == -1) {
		if (EINVAL == errno) {
			fprintf (stderr, "%s does not support memory mapping\n", dev_name);
			exit (EXIT_FAILURE);
		} else 
			errno_exit ("VIDIOC_REQBUFS");
	}

	if (req.count < 2) {
		fprintf (stderr, "Insufficient buffer memory on %s\n", dev_name);
		exit (EXIT_FAILURE);
	}

	buffers = calloc (req.count, sizeof (*buffers));

	if (!buffers) {
		fprintf (stderr, "Out of memory\n"); 
		exit (EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		struct v4l2_buffer buf;

		memset(&buf, 0, sizeof(buf));
		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = n_buffers;

		if (-1 == xioctl (fd, VIDIOC_QUERYBUF, &buf))
			errno_exit ("VIDIOC_QUERYBUF");

		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start = mmap(NULL /* start anywhere */,
					buf.length,
					PROT_READ | PROT_WRITE /* required */,
					MAP_SHARED /* recommended */,
					fd, buf.m.offset);

		if (MAP_FAILED == buffers[n_buffers].start)
			errno_exit ("mmap");
	}
}

static void init_device(void)
{
	struct v4l2_capability cap;
	struct v4l2_format fmt;

	memset(&cap, 0, sizeof(cap));
	if (-1 == xioctl (fd, VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL == errno) {
			fprintf (stderr, "%s is no V4L2 device\n", dev_name);
			exit (EXIT_FAILURE);
		} else 
			errno_exit ("VIDIOC_QUERYCAP");
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		fprintf (stderr, "%s is no video capture device\n", dev_name);
		exit (EXIT_FAILURE);
	}

	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		fprintf (stderr, "%s does not support streaming i/o\n", dev_name);
		exit (EXIT_FAILURE);
	}

	memset(&fmt, 0, sizeof(fmt));
	fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width       = video_width;
	fmt.fmt.pix.height      = video_height;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;

	if (-1 == xioctl (fd, VIDIOC_S_FMT, &fmt))
		errno_exit ("VIDIOC_S_FMT");

	init_mmap ();
}

static void close_device(void)
{
  if (-1 == close (fd))
	  errno_exit ("close");
  fd = -1;

}

static void open_device(void)
{
	fd = open (dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);
	if (-1 == fd) {
		fprintf (stderr, "Cannot open '%s': %d, %s\n",
				dev_name, errno, strerror (errno));
		exit (EXIT_FAILURE);
	}
}

int main()
{
	open_device ();
	init_device ();

	if(init_view()) {
		close_device();
		return -1;
	}		

	start_capturing ();
	mainloop ();
	stop_capturing ();
	close_view();
	uninit_device ();
	close_device ();
	return 0;
}
