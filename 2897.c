static int vidioc_enum_fmt_out(struct file *file, void *fh,
			       struct v4l2_fmtdesc *f)
{
	struct v4l2_loopback_device *dev;
	const struct v4l2l_format *fmt;

	dev = v4l2loopback_getdevice(file);

	if (dev->ready_for_capture) {
		const __u32 format = dev->pix_format.pixelformat;

		/* format has been fixed by the writer, so only one single format is supported */
		if (f->index)
			return -EINVAL;

		fmt = format_by_fourcc(format);
		if (NULL == fmt)
			return -EINVAL;

		f->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		/* f->flags = ??; */
		snprintf(f->description, sizeof(f->description), "%s",
			 fmt->name);

		f->pixelformat = dev->pix_format.pixelformat;
	} else {
		/* fill in a dummy format */
		/* coverity[unsigned_compare] */
		if (f->index < 0 || f->index >= FORMATS)
			return -EINVAL;

		fmt = &formats[f->index];

		f->pixelformat = fmt->fourcc;
		snprintf(f->description, sizeof(f->description), "%s",
			 fmt->name);
	}
	f->flags = 0;

	return 0;
}