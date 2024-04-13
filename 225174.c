static int v4l_g_parm(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct v4l2_streamparm *p = arg;
	v4l2_std_id std;
	int ret = check_fmt(file, p->type);

	if (ret)
		return ret;
	if (ops->vidioc_g_parm)
		return ops->vidioc_g_parm(file, fh, p);
	if (p->type != V4L2_BUF_TYPE_VIDEO_CAPTURE &&
	    p->type != V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE)
		return -EINVAL;
	p->parm.capture.readbuffers = 2;
	ret = ops->vidioc_g_std(file, fh, &std);
	if (ret == 0)
		v4l2_video_std_frame_period(std, &p->parm.capture.timeperframe);
	return ret;
}