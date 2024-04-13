static int v4l_unsubscribe_event(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	return ops->vidioc_unsubscribe_event(fh, arg);
}