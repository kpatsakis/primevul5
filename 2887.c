static ssize_t attr_show_maxopeners(struct device *cd,
				    struct device_attribute *attr, char *buf)
{
	struct v4l2_loopback_device *dev = v4l2loopback_cd2dev(cd);

	return sprintf(buf, "%d\n", dev->max_openers);
}