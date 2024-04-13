static int v4l_s_selection(const struct v4l2_ioctl_ops *ops,
			   struct file *file, void *fh, void *arg)
{
	struct v4l2_selection *p = arg;
	u32 old_type = p->type;
	int ret;

	if (p->type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE)
		p->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	else if (p->type == V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE)
		p->type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ops->vidioc_s_selection(file, fh, p);
	p->type = old_type;
	return ret;
}