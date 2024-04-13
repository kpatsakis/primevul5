static int vidioc_s_parm(struct file *file, void *priv,
			 struct v4l2_streamparm *parm)
{
	struct v4l2_loopback_device *dev;
	int err = 0;
	MARK();

	dev = v4l2loopback_getdevice(file);
	dprintk("vidioc_s_parm called frate=%d/%d\n",
		parm->parm.capture.timeperframe.numerator,
		parm->parm.capture.timeperframe.denominator);

	switch (parm->type) {
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		if ((err = set_timeperframe(
			     dev, &parm->parm.capture.timeperframe)) < 0)
			return err;
		break;
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
		if ((err = set_timeperframe(
			     dev, &parm->parm.capture.timeperframe)) < 0)
			return err;
		break;
	default:
		return -1;
	}

	parm->parm.capture = dev->capture_param;
	return 0;
}