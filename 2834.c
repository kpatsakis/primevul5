static int vidioc_try_fmt_out(struct file *file, void *priv,
			      struct v4l2_format *fmt)
{
	struct v4l2_loopback_device *dev;
	MARK();

	dev = v4l2loopback_getdevice(file);

	/* TODO(vasaka) loopback does not care about formats writer want to set,
	 * maybe it is a good idea to restrict format somehow */
	if (dev->ready_for_capture) {
		fmt->fmt.pix = dev->pix_format;
	} else {
		__u32 w = fmt->fmt.pix.width;
		__u32 h = fmt->fmt.pix.height;
		__u32 pixfmt = fmt->fmt.pix.pixelformat;
		const struct v4l2l_format *format = format_by_fourcc(pixfmt);

		if (w > dev->max_width)
			w = dev->max_width;
		if (h > dev->max_height)
			h = dev->max_height;

		dprintk("trying image %dx%d\n", w, h);

		if (w < 1)
			w = V4L2LOOPBACK_SIZE_DEFAULT_WIDTH;

		if (h < 1)
			h = V4L2LOOPBACK_SIZE_DEFAULT_HEIGHT;

		if (NULL == format)
			format = &formats[0];

		pix_format_set_size(&fmt->fmt.pix, format, w, h);

		fmt->fmt.pix.pixelformat = format->fourcc;

		if ((fmt->fmt.pix.colorspace == V4L2_COLORSPACE_DEFAULT) ||
		    (fmt->fmt.pix.colorspace > V4L2_COLORSPACE_DCI_P3))
			fmt->fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;

		if (V4L2_FIELD_ANY == fmt->fmt.pix.field)
			fmt->fmt.pix.field = V4L2_FIELD_NONE;

		/* FIXXME: try_fmt should never modify the device-state */
		dev->pix_format = fmt->fmt.pix;
	}
	return 0;
}