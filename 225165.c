static int v4l_g_sliced_vbi_cap(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct v4l2_sliced_vbi_cap *p = arg;
	int ret = check_fmt(file, p->type);

	if (ret)
		return ret;

	/* Clear up to type, everything after type is zeroed already */
	memset(p, 0, offsetof(struct v4l2_sliced_vbi_cap, type));

	return ops->vidioc_g_sliced_vbi_cap(file, fh, p);
}