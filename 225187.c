static int v4l_create_bufs(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct v4l2_create_buffers *create = arg;
	int ret = check_fmt(file, create->format.type);

	if (ret)
		return ret;

	CLEAR_AFTER_FIELD(create, capabilities);

	v4l_sanitize_format(&create->format);

	ret = ops->vidioc_create_bufs(file, fh, create);

	if (create->format.type == V4L2_BUF_TYPE_VIDEO_CAPTURE ||
	    create->format.type == V4L2_BUF_TYPE_VIDEO_OUTPUT)
		create->format.fmt.pix.priv = V4L2_PIX_FMT_PRIV_MAGIC;

	return ret;
}