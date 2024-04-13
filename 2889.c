static int vidioc_queryctrl(struct file *file, void *fh,
			    struct v4l2_queryctrl *q)
{
	const struct v4l2_ctrl_config *cnf = 0;
	switch (q->id) {
	case CID_KEEP_FORMAT:
		cnf = &v4l2loopback_ctrl_keepformat;
		break;
	case CID_SUSTAIN_FRAMERATE:
		cnf = &v4l2loopback_ctrl_sustainframerate;
		break;
	case CID_TIMEOUT:
		cnf = &v4l2loopback_ctrl_timeout;
		break;
	case CID_TIMEOUT_IMAGE_IO:
		cnf = &v4l2loopback_ctrl_timeoutimageio;
		break;
	default:
		return -EINVAL;
	}
	if (!cnf)
		BUG();

	strcpy(q->name, cnf->name);
	q->default_value = cnf->def;
	q->type = cnf->type;
	q->minimum = cnf->min;
	q->maximum = cnf->max;
	q->step = cnf->step;

	memset(q->reserved, 0, sizeof(q->reserved));
	return 0;
}