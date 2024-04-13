static int vidioc_s_fmt_cap(struct file *file, void *priv,
			    struct v4l2_format *fmt)
{
	return vidioc_try_fmt_cap(file, priv, fmt);
}