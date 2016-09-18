#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <linux/videodev2.h>

static struct v4l2_capability v4l_caps;
/*
static struct v4l2_format v4l_fmt;
static struct v4l2_requestbuffers v4l_breq;
static struct v4l2_buffer v4l_buf;
*/
int main(int argc, char **argv)
{
	int fd;
	unsigned int caps;
	char *dev_name = "/dev/video0";

	if (argc == 2)
		dev_name = argv[1];

	printf("Using device: %s\n", dev_name);

	if ((fd = open(dev_name, O_RDONLY)) < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	if (ioctl(fd, VIDIOC_QUERYCAP, &v4l_caps) == -1) {
		perror("VIDIOC_QUERYCAP");
		exit(EXIT_FAILURE);
	}

	caps = v4l_caps.capabilities;
	if (!(caps & V4L2_CAP_VIDEO_CAPTURE) || !(caps & V4L2_CAP_STREAMING)) {
		fprintf(stderr, "This device does not have CAP and STREAMING. Aborting\n");
		exit(EXIT_FAILURE);
	}

	if (caps & V4L2_CAP_VIDEO_OVERLAY)
		printf("Overlay supported\n");

/*
	v4l_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	v4l_fmt.fmt.pix.width = 640;
	v4l_fmt.fmt.pix.height = 480;
	if (ioctl(fd, VIDIOC_S_FMT, &v4l_fmt) == -1) {
		perror("VIDIOC_S_FMT");
		exit(EXIT_FAILURE);
	}

	v4l_breq.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l_breq.memory = V4L2_MEMORY_MMAP;
	v4l_breq.count = 1;
	if (ioctl(fd, VIDIOC_REQBUFS, &v4l_breq) == -1) {
		perror("VIDIOC_REQBUFS");
		exit(EXIT_FAILURE);
	}

	memset(&v4l_buf, 0, sizeof(v4l_buf));
	v4l_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l_buf.memory = V4L2_MEMORY_MMAP;
*/

	close(fd);

	exit(EXIT_SUCCESS);
}
