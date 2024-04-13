static int vidioc_g_fmt_out(struct file *file, void *priv,
			    struct v4l2_format *fmt)
{
	struct v4l2_loopback_device *dev;
	MARK();

	dev = v4l2loopback_getdevice(file);

	/*
	 * LATER: this should return the currently valid format
	 * gstreamer doesn't like it, if this returns -EINVAL, as it
	 * then concludes that there is _no_ valid format
	 * CHECK whether this assumption is wrong,
	 * or whether we have to always provide a valid format
	 */

	fmt->fmt.pix = dev->pix_format;
	return 0;
}