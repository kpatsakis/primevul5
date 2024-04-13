static int v4l_s_modulator(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct video_device *vfd = video_devdata(file);
	struct v4l2_modulator *p = arg;

	if (vfd->vfl_type == VFL_TYPE_RADIO)
		p->type = V4L2_TUNER_RADIO;

	return ops->vidioc_s_modulator(file, fh, p);
}