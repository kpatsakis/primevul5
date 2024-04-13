static int vidioc_enum_frameintervals(struct file *file, void *fh,
				      struct v4l2_frmivalenum *argp)
{
	struct v4l2_loopback_device *dev = v4l2loopback_getdevice(file);
	struct v4l2_loopback_opener *opener = fh_to_opener(fh);

	if (dev->ready_for_capture) {
		if (opener->vidioc_enum_frameintervals_calls > 0)
			return -EINVAL;
		if (argp->width == dev->pix_format.width &&
		    argp->height == dev->pix_format.height) {
			argp->type = V4L2_FRMIVAL_TYPE_DISCRETE;
			argp->discrete = dev->capture_param.timeperframe;
			opener->vidioc_enum_frameintervals_calls++;
			return 0;
		}
		return -EINVAL;
	}
	return 0;
}