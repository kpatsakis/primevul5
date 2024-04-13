static int v4l_dqevent(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	return v4l2_event_dequeue(fh, arg, file->f_flags & O_NONBLOCK);
}