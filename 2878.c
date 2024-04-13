static int vidioc_streamon(struct file *file, void *fh, enum v4l2_buf_type type)
{
	struct v4l2_loopback_device *dev;
	struct v4l2_loopback_opener *opener;
	MARK();

	dev = v4l2loopback_getdevice(file);
	opener = fh_to_opener(fh);

	switch (type) {
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
		if (!dev->ready_for_capture) {
			int ret = allocate_buffers(dev);
			if (ret < 0)
				return ret;
		}
		opener->type = WRITER;
		dev->ready_for_output = 0;
		dev->ready_for_capture++;
		return 0;
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		if (!dev->ready_for_capture)
			return -EIO;
		opener->type = READER;
		return 0;
	default:
		return -EINVAL;
	}
	return -EINVAL;
}