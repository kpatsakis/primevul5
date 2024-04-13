static int v4l_log_status(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct video_device *vfd = video_devdata(file);
	int ret;

	if (vfd->v4l2_dev)
		pr_info("%s: =================  START STATUS  =================\n",
			vfd->v4l2_dev->name);
	ret = ops->vidioc_log_status(file, fh);
	if (vfd->v4l2_dev)
		pr_info("%s: ==================  END STATUS  ==================\n",
			vfd->v4l2_dev->name);
	return ret;
}