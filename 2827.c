static int vidioc_g_ctrl(struct file *file, void *fh, struct v4l2_control *c)
{
	struct v4l2_loopback_device *dev = v4l2loopback_getdevice(file);

	switch (c->id) {
	case CID_KEEP_FORMAT:
		c->value = dev->keep_format;
		break;
	case CID_SUSTAIN_FRAMERATE:
		c->value = dev->sustain_framerate;
		break;
	case CID_TIMEOUT:
		c->value = jiffies_to_msecs(dev->timeout_jiffies);
		break;
	case CID_TIMEOUT_IMAGE_IO:
		c->value = dev->timeout_image_io;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}