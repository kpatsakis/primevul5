static int v4l_streamon(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	return ops->vidioc_streamon(file, fh, *(unsigned int *)arg);
}