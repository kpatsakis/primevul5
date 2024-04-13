static int vidioc_g_fmt_cap(struct file *file, void *priv,
			    struct v4l2_format *fmt)
{
	struct v4l2_loopback_device *dev;
	MARK();

	dev = v4l2loopback_getdevice(file);

	if (!dev->ready_for_capture)
		return -EINVAL;

	fmt->fmt.pix = dev->pix_format;
	MARK();
	return 0;
}