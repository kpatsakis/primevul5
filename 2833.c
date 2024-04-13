static int vidioc_enum_framesizes(struct file *file, void *fh,
				  struct v4l2_frmsizeenum *argp)
{
	struct v4l2_loopback_device *dev;

	/* LATER: what does the index really  mean?
	 * if it's about enumerating formats, we can safely ignore it
	 * (CHECK)
	 */

	/* there can be only one... */
	if (argp->index)
		return -EINVAL;

	dev = v4l2loopback_getdevice(file);
	if (dev->ready_for_capture) {
		/* format has already been negotiated
		 * cannot change during runtime
		 */
		argp->type = V4L2_FRMSIZE_TYPE_DISCRETE;

		argp->discrete.width = dev->pix_format.width;
		argp->discrete.height = dev->pix_format.height;
	} else {
		/* if the format has not been negotiated yet, we accept anything
		 */
		argp->type = V4L2_FRMSIZE_TYPE_CONTINUOUS;

		argp->stepwise.min_width = V4L2LOOPBACK_SIZE_MIN_WIDTH;
		argp->stepwise.min_height = V4L2LOOPBACK_SIZE_MIN_HEIGHT;

		argp->stepwise.max_width = dev->max_width;
		argp->stepwise.max_height = dev->max_height;

		argp->stepwise.step_width = 1;
		argp->stepwise.step_height = 1;
	}
	return 0;
}