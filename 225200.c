static int v4l_dbg_g_register(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
#ifdef CONFIG_VIDEO_ADV_DEBUG
	struct v4l2_dbg_register *p = arg;
	struct video_device *vfd = video_devdata(file);
	struct v4l2_subdev *sd;
	int idx = 0;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;
	if (p->match.type == V4L2_CHIP_MATCH_SUBDEV) {
		if (vfd->v4l2_dev == NULL)
			return -EINVAL;
		v4l2_device_for_each_subdev(sd, vfd->v4l2_dev)
			if (p->match.addr == idx++)
				return v4l2_subdev_call(sd, core, g_register, p);
		return -EINVAL;
	}
	if (ops->vidioc_g_register && p->match.type == V4L2_CHIP_MATCH_BRIDGE &&
	    (ops->vidioc_g_chip_info || p->match.addr == 0))
		return ops->vidioc_g_register(file, fh, p);
	return -EINVAL;
#else
	return -ENOTTY;
#endif
}