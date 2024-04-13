static int vidioc_try_fmt_cap(struct file *file, void *priv,
			      struct v4l2_format *fmt)
{
	struct v4l2_loopback_device *dev;
	char buf[5];

	dev = v4l2loopback_getdevice(file);

	if (0 == dev->ready_for_capture) {
		dprintk("setting fmt_cap not possible yet\n");
		return -EBUSY;
	}

	if (fmt->fmt.pix.pixelformat != dev->pix_format.pixelformat)
		return -EINVAL;

	fmt->fmt.pix = dev->pix_format;

	buf[4] = 0;
	dprintk("capFOURCC=%s\n", fourcc2str(dev->pix_format.pixelformat, buf));
	return 0;
}