static int v4l_reqbufs(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct v4l2_requestbuffers *p = arg;
	int ret = check_fmt(file, p->type);

	if (ret)
		return ret;

	CLEAR_AFTER_FIELD(p, capabilities);

	return ops->vidioc_reqbufs(file, fh, p);
}