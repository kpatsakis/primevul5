static int v4l_s_ext_ctrls(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct video_device *vfd = video_devdata(file);
	struct v4l2_ext_controls *p = arg;
	struct v4l2_fh *vfh =
		test_bit(V4L2_FL_USES_V4L2_FH, &vfd->flags) ? fh : NULL;

	p->error_idx = p->count;
	if (vfh && vfh->ctrl_handler)
		return v4l2_s_ext_ctrls(vfh, vfh->ctrl_handler,
					vfd, vfd->v4l2_dev->mdev, p);
	if (vfd->ctrl_handler)
		return v4l2_s_ext_ctrls(NULL, vfd->ctrl_handler,
					vfd, vfd->v4l2_dev->mdev, p);
	if (ops->vidioc_s_ext_ctrls == NULL)
		return -ENOTTY;
	return check_ext_ctrls(p, 0) ? ops->vidioc_s_ext_ctrls(file, fh, p) :
					-EINVAL;
}