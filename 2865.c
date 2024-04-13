static int vidioc_g_output(struct file *file, void *fh, unsigned int *i)
{
	struct v4l2_loopback_device *dev = v4l2loopback_getdevice(file);
	if (!dev->announce_all_caps && !dev->ready_for_output)
		return -ENOTTY;
	if (i)
		*i = 0;
	return 0;
}