static int v4l_dbg_g_chip_info(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
#ifdef CONFIG_VIDEO_ADV_DEBUG
	struct video_device *vfd = video_devdata(file);
	struct v4l2_dbg_chip_info *p = arg;
	struct v4l2_subdev *sd;
	int idx = 0;

	switch (p->match.type) {
	case V4L2_CHIP_MATCH_BRIDGE:
		if (ops->vidioc_s_register)
			p->flags |= V4L2_CHIP_FL_WRITABLE;
		if (ops->vidioc_g_register)
			p->flags |= V4L2_CHIP_FL_READABLE;
		strscpy(p->name, vfd->v4l2_dev->name, sizeof(p->name));
		if (ops->vidioc_g_chip_info)
			return ops->vidioc_g_chip_info(file, fh, arg);
		if (p->match.addr)
			return -EINVAL;
		return 0;

	case V4L2_CHIP_MATCH_SUBDEV:
		if (vfd->v4l2_dev == NULL)
			break;
		v4l2_device_for_each_subdev(sd, vfd->v4l2_dev) {
			if (p->match.addr != idx++)
				continue;
			if (sd->ops->core && sd->ops->core->s_register)
				p->flags |= V4L2_CHIP_FL_WRITABLE;
			if (sd->ops->core && sd->ops->core->g_register)
				p->flags |= V4L2_CHIP_FL_READABLE;
			strscpy(p->name, sd->name, sizeof(p->name));
			return 0;
		}
		break;
	}
	return -EINVAL;
#else
	return -ENOTTY;
#endif
}