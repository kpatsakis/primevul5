static int vidioc_streamoff(struct file *file, void *fh,
			    enum v4l2_buf_type type)
{
	struct v4l2_loopback_device *dev;
	MARK();
	dprintk("%d\n", type);

	dev = v4l2loopback_getdevice(file);

	switch (type) {
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
		if (dev->ready_for_capture > 0)
			dev->ready_for_capture--;
		return 0;
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		return 0;
	default:
		return -EINVAL;
	}
	return -EINVAL;
}