static int v4l_s_output(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct video_device *vfd = video_devdata(file);

	if (vfd->device_caps & V4L2_CAP_IO_MC)
		return  *(int *)arg ? -EINVAL : 0;

	return ops->vidioc_s_output(file, fh, *(unsigned int *)arg);
}