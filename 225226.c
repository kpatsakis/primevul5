static int v4l_g_input(const struct v4l2_ioctl_ops *ops,
		       struct file *file, void *fh, void *arg)
{
	struct video_device *vfd = video_devdata(file);

	if (vfd->device_caps & V4L2_CAP_IO_MC) {
		*(int *)arg = 0;
		return 0;
	}

	return ops->vidioc_g_input(file, fh, arg);
}