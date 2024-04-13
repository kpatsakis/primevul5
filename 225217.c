static int v4l_s_frequency(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct video_device *vfd = video_devdata(file);
	const struct v4l2_frequency *p = arg;
	enum v4l2_tuner_type type;
	int ret;

	ret = v4l_enable_media_source(vfd);
	if (ret)
		return ret;
	if (vfd->vfl_type == VFL_TYPE_SDR) {
		if (p->type != V4L2_TUNER_SDR && p->type != V4L2_TUNER_RF)
			return -EINVAL;
	} else {
		type = (vfd->vfl_type == VFL_TYPE_RADIO) ?
				V4L2_TUNER_RADIO : V4L2_TUNER_ANALOG_TV;
		if (type != p->type)
			return -EINVAL;
	}
	return ops->vidioc_s_frequency(file, fh, p);
}