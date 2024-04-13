static int v4l_dqbuf(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct v4l2_buffer *p = arg;
	int ret = check_fmt(file, p->type);

	return ret ? ret : ops->vidioc_dqbuf(file, fh, p);
}