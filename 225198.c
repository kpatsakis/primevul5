static int v4l_querymenu(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct video_device *vfd = video_devdata(file);
	struct v4l2_querymenu *p = arg;
	struct v4l2_fh *vfh =
		test_bit(V4L2_FL_USES_V4L2_FH, &vfd->flags) ? fh : NULL;

	if (vfh && vfh->ctrl_handler)
		return v4l2_querymenu(vfh->ctrl_handler, p);
	if (vfd->ctrl_handler)
		return v4l2_querymenu(vfd->ctrl_handler, p);
	if (ops->vidioc_querymenu)
		return ops->vidioc_querymenu(file, fh, p);
	return -ENOTTY;
}