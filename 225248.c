static int v4l_s_tuner(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct video_device *vfd = video_devdata(file);
	struct v4l2_tuner *p = arg;
	int ret;

	ret = v4l_enable_media_source(vfd);
	if (ret)
		return ret;
	p->type = (vfd->vfl_type == VFL_TYPE_RADIO) ?
			V4L2_TUNER_RADIO : V4L2_TUNER_ANALOG_TV;
	return ops->vidioc_s_tuner(file, fh, p);
}