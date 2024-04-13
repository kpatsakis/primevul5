static int v4l_s_parm(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct v4l2_streamparm *p = arg;
	int ret = check_fmt(file, p->type);

	if (ret)
		return ret;

	/* Note: extendedmode is never used in drivers */
	if (V4L2_TYPE_IS_OUTPUT(p->type)) {
		memset(p->parm.output.reserved, 0,
		       sizeof(p->parm.output.reserved));
		p->parm.output.extendedmode = 0;
		p->parm.output.outputmode &= V4L2_MODE_HIGHQUALITY;
	} else {
		memset(p->parm.capture.reserved, 0,
		       sizeof(p->parm.capture.reserved));
		p->parm.capture.extendedmode = 0;
		p->parm.capture.capturemode &= V4L2_MODE_HIGHQUALITY;
	}
	return ops->vidioc_s_parm(file, fh, p);
}