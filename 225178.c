static int v4l_g_ctrl(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct video_device *vfd = video_devdata(file);
	struct v4l2_control *p = arg;
	struct v4l2_fh *vfh =
		test_bit(V4L2_FL_USES_V4L2_FH, &vfd->flags) ? fh : NULL;
	struct v4l2_ext_controls ctrls;
	struct v4l2_ext_control ctrl;

	if (vfh && vfh->ctrl_handler)
		return v4l2_g_ctrl(vfh->ctrl_handler, p);
	if (vfd->ctrl_handler)
		return v4l2_g_ctrl(vfd->ctrl_handler, p);
	if (ops->vidioc_g_ctrl)
		return ops->vidioc_g_ctrl(file, fh, p);
	if (ops->vidioc_g_ext_ctrls == NULL)
		return -ENOTTY;

	ctrls.which = V4L2_CTRL_ID2WHICH(p->id);
	ctrls.count = 1;
	ctrls.controls = &ctrl;
	ctrl.id = p->id;
	ctrl.value = p->value;
	if (check_ext_ctrls(&ctrls, 1)) {
		int ret = ops->vidioc_g_ext_ctrls(file, fh, &ctrls);

		if (ret == 0)
			p->value = ctrl.value;
		return ret;
	}
	return -EINVAL;
}