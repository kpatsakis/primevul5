static int v4l_g_crop(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct video_device *vfd = video_devdata(file);
	struct v4l2_crop *p = arg;
	struct v4l2_selection s = {
		.type = p->type,
	};
	int ret;

	/* simulate capture crop using selection api */

	/* crop means compose for output devices */
	if (V4L2_TYPE_IS_OUTPUT(p->type))
		s.target = V4L2_SEL_TGT_COMPOSE;
	else
		s.target = V4L2_SEL_TGT_CROP;

	if (test_bit(V4L2_FL_QUIRK_INVERTED_CROP, &vfd->flags))
		s.target = s.target == V4L2_SEL_TGT_COMPOSE ?
			V4L2_SEL_TGT_CROP : V4L2_SEL_TGT_COMPOSE;

	ret = v4l_g_selection(ops, file, fh, &s);

	/* copying results to old structure on success */
	if (!ret)
		p->c = s.r;
	return ret;
}