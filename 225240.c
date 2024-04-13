static int v4l_s_priority(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct video_device *vfd;
	struct v4l2_fh *vfh;
	u32 *p = arg;

	vfd = video_devdata(file);
	if (!test_bit(V4L2_FL_USES_V4L2_FH, &vfd->flags))
		return -ENOTTY;
	vfh = file->private_data;
	return v4l2_prio_change(vfd->prio, &vfh->prio, *p);
}