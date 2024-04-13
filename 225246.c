static int v4l_querystd(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct video_device *vfd = video_devdata(file);
	v4l2_std_id *p = arg;
	int ret;

	ret = v4l_enable_media_source(vfd);
	if (ret)
		return ret;
	/*
	 * If no signal is detected, then the driver should return
	 * V4L2_STD_UNKNOWN. Otherwise it should return tvnorms with
	 * any standards that do not apply removed.
	 *
	 * This means that tuners, audio and video decoders can join
	 * their efforts to improve the standards detection.
	 */
	*p = vfd->tvnorms;
	return ops->vidioc_querystd(file, fh, arg);
}