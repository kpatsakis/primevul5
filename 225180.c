static int v4l_s_hw_freq_seek(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct video_device *vfd = video_devdata(file);
	struct v4l2_hw_freq_seek *p = arg;
	enum v4l2_tuner_type type;
	int ret;

	ret = v4l_enable_media_source(vfd);
	if (ret)
		return ret;
	/* s_hw_freq_seek is not supported for SDR for now */
	if (vfd->vfl_type == VFL_TYPE_SDR)
		return -EINVAL;

	type = (vfd->vfl_type == VFL_TYPE_RADIO) ?
		V4L2_TUNER_RADIO : V4L2_TUNER_ANALOG_TV;
	if (p->type != type)
		return -EINVAL;
	return ops->vidioc_s_hw_freq_seek(file, fh, p);
}