static int v4l_subscribe_event(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	return ops->vidioc_subscribe_event(fh, arg);
}