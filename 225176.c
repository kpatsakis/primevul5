static int v4l_query_ext_ctrl(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct video_device *vfd = video_devdata(file);
	struct v4l2_query_ext_ctrl *p = arg;
	struct v4l2_fh *vfh =
		test_bit(V4L2_FL_USES_V4L2_FH, &vfd->flags) ? fh : NULL;

	if (vfh && vfh->ctrl_handler)
		return v4l2_query_ext_ctrl(vfh->ctrl_handler, p);
	if (vfd->ctrl_handler)
		return v4l2_query_ext_ctrl(vfd->ctrl_handler, p);
	if (ops->vidioc_query_ext_ctrl)
		return ops->vidioc_query_ext_ctrl(file, fh, p);
	return -ENOTTY;
}