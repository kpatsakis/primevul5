static ssize_t attr_store_format(struct device *cd,
				 struct device_attribute *attr, const char *buf,
				 size_t len)
{
	struct v4l2_loopback_device *dev = v4l2loopback_cd2dev(cd);
	int fps_num = 0, fps_den = 1;

	if (!dev)
		return -ENODEV;

	/* only fps changing is supported */
	if (sscanf(buf, "@%d/%d", &fps_num, &fps_den) > 0) {
		struct v4l2_fract f = { .numerator = fps_den,
					.denominator = fps_num };
		int err = 0;
		if ((err = set_timeperframe(dev, &f)) < 0)
			return err;
		return len;
	}
	return -EINVAL;
}