static int v4l_enum_fmt(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct video_device *vdev = video_devdata(file);
	struct v4l2_fmtdesc *p = arg;
	int ret = check_fmt(file, p->type);
	u32 mbus_code;
	u32 cap_mask;

	if (ret)
		return ret;
	ret = -EINVAL;

	if (!(vdev->device_caps & V4L2_CAP_IO_MC))
		p->mbus_code = 0;

	mbus_code = p->mbus_code;
	CLEAR_AFTER_FIELD(p, type);
	p->mbus_code = mbus_code;

	switch (p->type) {
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
	case V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE:
		cap_mask = V4L2_CAP_VIDEO_CAPTURE_MPLANE |
			   V4L2_CAP_VIDEO_M2M_MPLANE;
		if (!!(vdev->device_caps & cap_mask) !=
		    (p->type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE))
			break;

		if (unlikely(!ops->vidioc_enum_fmt_vid_cap))
			break;
		ret = ops->vidioc_enum_fmt_vid_cap(file, fh, arg);
		break;
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
		if (unlikely(!ops->vidioc_enum_fmt_vid_overlay))
			break;
		ret = ops->vidioc_enum_fmt_vid_overlay(file, fh, arg);
		break;
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
	case V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE:
		cap_mask = V4L2_CAP_VIDEO_OUTPUT_MPLANE |
			   V4L2_CAP_VIDEO_M2M_MPLANE;
		if (!!(vdev->device_caps & cap_mask) !=
		    (p->type == V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE))
			break;

		if (unlikely(!ops->vidioc_enum_fmt_vid_out))
			break;
		ret = ops->vidioc_enum_fmt_vid_out(file, fh, arg);
		break;
	case V4L2_BUF_TYPE_SDR_CAPTURE:
		if (unlikely(!ops->vidioc_enum_fmt_sdr_cap))
			break;
		ret = ops->vidioc_enum_fmt_sdr_cap(file, fh, arg);
		break;
	case V4L2_BUF_TYPE_SDR_OUTPUT:
		if (unlikely(!ops->vidioc_enum_fmt_sdr_out))
			break;
		ret = ops->vidioc_enum_fmt_sdr_out(file, fh, arg);
		break;
	case V4L2_BUF_TYPE_META_CAPTURE:
		if (unlikely(!ops->vidioc_enum_fmt_meta_cap))
			break;
		ret = ops->vidioc_enum_fmt_meta_cap(file, fh, arg);
		break;
	case V4L2_BUF_TYPE_META_OUTPUT:
		if (unlikely(!ops->vidioc_enum_fmt_meta_out))
			break;
		ret = ops->vidioc_enum_fmt_meta_out(file, fh, arg);
		break;
	}
	if (ret == 0)
		v4l_fill_fmtdesc(p);
	return ret;
}