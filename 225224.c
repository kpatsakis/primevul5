static int v4l_g_modulator(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct video_device *vfd = video_devdata(file);
	struct v4l2_modulator *p = arg;
	int err;

	if (vfd->vfl_type == VFL_TYPE_RADIO)
		p->type = V4L2_TUNER_RADIO;

	err = ops->vidioc_g_modulator(file, fh, p);
	if (!err)
		p->capability |= V4L2_TUNER_CAP_FREQ_BANDS;
	return err;
}