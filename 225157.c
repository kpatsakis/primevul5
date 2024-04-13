static int v4l_prepare_buf(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct v4l2_buffer *b = arg;
	int ret = check_fmt(file, b->type);

	return ret ? ret : ops->vidioc_prepare_buf(file, fh, b);
}