static int v4l_enum_freq_bands(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct video_device *vfd = video_devdata(file);
	struct v4l2_frequency_band *p = arg;
	enum v4l2_tuner_type type;
	int err;

	if (vfd->vfl_type == VFL_TYPE_SDR) {
		if (p->type != V4L2_TUNER_SDR && p->type != V4L2_TUNER_RF)
			return -EINVAL;
		type = p->type;
	} else {
		type = (vfd->vfl_type == VFL_TYPE_RADIO) ?
				V4L2_TUNER_RADIO : V4L2_TUNER_ANALOG_TV;
		if (type != p->type)
			return -EINVAL;
	}
	if (ops->vidioc_enum_freq_bands) {
		err = ops->vidioc_enum_freq_bands(file, fh, p);
		if (err != -ENOTTY)
			return err;
	}
	if (is_valid_ioctl(vfd, VIDIOC_G_TUNER)) {
		struct v4l2_tuner t = {
			.index = p->tuner,
			.type = type,
		};

		if (p->index)
			return -EINVAL;
		err = ops->vidioc_g_tuner(file, fh, &t);
		if (err)
			return err;
		p->capability = t.capability | V4L2_TUNER_CAP_FREQ_BANDS;
		p->rangelow = t.rangelow;
		p->rangehigh = t.rangehigh;
		p->modulation = (type == V4L2_TUNER_RADIO) ?
			V4L2_BAND_MODULATION_FM : V4L2_BAND_MODULATION_VSB;
		return 0;
	}
	if (is_valid_ioctl(vfd, VIDIOC_G_MODULATOR)) {
		struct v4l2_modulator m = {
			.index = p->tuner,
		};

		if (type != V4L2_TUNER_RADIO)
			return -EINVAL;
		if (p->index)
			return -EINVAL;
		err = ops->vidioc_g_modulator(file, fh, &m);
		if (err)
			return err;
		p->capability = m.capability | V4L2_TUNER_CAP_FREQ_BANDS;
		p->rangelow = m.rangelow;
		p->rangehigh = m.rangehigh;
		p->modulation = (type == V4L2_TUNER_RADIO) ?
			V4L2_BAND_MODULATION_FM : V4L2_BAND_MODULATION_VSB;
		return 0;
	}
	return -ENOTTY;
}