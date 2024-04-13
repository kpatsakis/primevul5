static ssize_t attr_show_buffers(struct device *cd,
				 struct device_attribute *attr, char *buf)
{
	struct v4l2_loopback_device *dev = v4l2loopback_cd2dev(cd);

	if (!dev)
		return -ENODEV;

	return sprintf(buf, "%d\n", dev->used_buffers);
}