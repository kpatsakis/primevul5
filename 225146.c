static int v4l_s_std(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct video_device *vfd = video_devdata(file);
	v4l2_std_id id = *(v4l2_std_id *)arg, norm;
	int ret;

	ret = v4l_enable_media_source(vfd);
	if (ret)
		return ret;
	norm = id & vfd->tvnorms;
	if (vfd->tvnorms && !norm)	/* Check if std is supported */
		return -EINVAL;

	/* Calls the specific handler */
	return ops->vidioc_s_std(file, fh, norm);
}