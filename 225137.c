static int v4l_g_priority(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct video_device *vfd;
	u32 *p = arg;

	vfd = video_devdata(file);
	*p = v4l2_prio_max(vfd->prio);
	return 0;
}