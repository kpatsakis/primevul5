static int set_timeperframe(struct v4l2_loopback_device *dev,
			    struct v4l2_fract *tpf)
{
	if ((tpf->denominator < 1) || (tpf->numerator < 1)) {
		return -EINVAL;
	}
	dev->capture_param.timeperframe = *tpf;
	dev->frame_jiffies = max(1UL, msecs_to_jiffies(1000) * tpf->numerator /
					      tpf->denominator);
	return 0;
}