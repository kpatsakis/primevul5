static int vidioc_enum_fmt_cap(struct file *file, void *fh,
			       struct v4l2_fmtdesc *f)
{
	struct v4l2_loopback_device *dev;
	MARK();

	dev = v4l2loopback_getdevice(file);

	if (f->index)
		return -EINVAL;
	if (dev->ready_for_capture) {
		const __u32 format = dev->pix_format.pixelformat;

		snprintf(f->description, sizeof(f->description), "[%c%c%c%c]",
			 (format >> 0) & 0xFF, (format >> 8) & 0xFF,
			 (format >> 16) & 0xFF, (format >> 24) & 0xFF);

		f->pixelformat = dev->pix_format.pixelformat;
	} else {
		return -EINVAL;
	}
	f->flags = 0;
	MARK();
	return 0;
}