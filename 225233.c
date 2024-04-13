static int v4l_g_frequency(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct video_device *vfd = video_devdata(file);
	struct v4l2_frequency *p = arg;

	if (vfd->vfl_type == VFL_TYPE_SDR)
		p->type = V4L2_TUNER_SDR;
	else
		p->type = (vfd->vfl_type == VFL_TYPE_RADIO) ?
				V4L2_TUNER_RADIO : V4L2_TUNER_ANALOG_TV;
	return ops->vidioc_g_frequency(file, fh, p);
}