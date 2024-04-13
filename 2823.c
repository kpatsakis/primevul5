static int vidioc_s_fmt_out(struct file *file, void *priv,
			    struct v4l2_format *fmt)
{
	struct v4l2_loopback_device *dev;
	char buf[5];
	int ret;
	MARK();

	dev = v4l2loopback_getdevice(file);
	ret = vidioc_try_fmt_out(file, priv, fmt);

	dprintk("s_fmt_out(%d) %d...%d\n", ret, dev->ready_for_capture,
		dev->pix_format.sizeimage);

	buf[4] = 0;
	dprintk("outFOURCC=%s\n", fourcc2str(dev->pix_format.pixelformat, buf));

	if (ret < 0)
		return ret;

	if (!dev->ready_for_capture) {
		dev->buffer_size = PAGE_ALIGN(dev->pix_format.sizeimage);
		fmt->fmt.pix.sizeimage = dev->buffer_size;
		allocate_buffers(dev);
	}
	return ret;
}