static int check_fmt(struct file *file, enum v4l2_buf_type type)
{
	const u32 vid_caps = V4L2_CAP_VIDEO_CAPTURE |
			     V4L2_CAP_VIDEO_CAPTURE_MPLANE |
			     V4L2_CAP_VIDEO_OUTPUT |
			     V4L2_CAP_VIDEO_OUTPUT_MPLANE |
			     V4L2_CAP_VIDEO_M2M | V4L2_CAP_VIDEO_M2M_MPLANE;
	const u32 meta_caps = V4L2_CAP_META_CAPTURE |
			      V4L2_CAP_META_OUTPUT;
	struct video_device *vfd = video_devdata(file);
	const struct v4l2_ioctl_ops *ops = vfd->ioctl_ops;
	bool is_vid = vfd->vfl_type == VFL_TYPE_VIDEO &&
		      (vfd->device_caps & vid_caps);
	bool is_vbi = vfd->vfl_type == VFL_TYPE_VBI;
	bool is_sdr = vfd->vfl_type == VFL_TYPE_SDR;
	bool is_tch = vfd->vfl_type == VFL_TYPE_TOUCH;
	bool is_meta = vfd->vfl_type == VFL_TYPE_VIDEO &&
		       (vfd->device_caps & meta_caps);
	bool is_rx = vfd->vfl_dir != VFL_DIR_TX;
	bool is_tx = vfd->vfl_dir != VFL_DIR_RX;

	if (ops == NULL)
		return -EINVAL;

	switch (type) {
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		if ((is_vid || is_tch) && is_rx &&
		    (ops->vidioc_g_fmt_vid_cap || ops->vidioc_g_fmt_vid_cap_mplane))
			return 0;
		break;
	case V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE:
		if ((is_vid || is_tch) && is_rx && ops->vidioc_g_fmt_vid_cap_mplane)
			return 0;
		break;
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
		if (is_vid && is_rx && ops->vidioc_g_fmt_vid_overlay)
			return 0;
		break;
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
		if (is_vid && is_tx &&
		    (ops->vidioc_g_fmt_vid_out || ops->vidioc_g_fmt_vid_out_mplane))
			return 0;
		break;
	case V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE:
		if (is_vid && is_tx && ops->vidioc_g_fmt_vid_out_mplane)
			return 0;
		break;
	case V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY:
		if (is_vid && is_tx && ops->vidioc_g_fmt_vid_out_overlay)
			return 0;
		break;
	case V4L2_BUF_TYPE_VBI_CAPTURE:
		if (is_vbi && is_rx && ops->vidioc_g_fmt_vbi_cap)
			return 0;
		break;
	case V4L2_BUF_TYPE_VBI_OUTPUT:
		if (is_vbi && is_tx && ops->vidioc_g_fmt_vbi_out)
			return 0;
		break;
	case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
		if (is_vbi && is_rx && ops->vidioc_g_fmt_sliced_vbi_cap)
			return 0;
		break;
	case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
		if (is_vbi && is_tx && ops->vidioc_g_fmt_sliced_vbi_out)
			return 0;
		break;
	case V4L2_BUF_TYPE_SDR_CAPTURE:
		if (is_sdr && is_rx && ops->vidioc_g_fmt_sdr_cap)
			return 0;
		break;
	case V4L2_BUF_TYPE_SDR_OUTPUT:
		if (is_sdr && is_tx && ops->vidioc_g_fmt_sdr_out)
			return 0;
		break;
	case V4L2_BUF_TYPE_META_CAPTURE:
		if (is_meta && is_rx && ops->vidioc_g_fmt_meta_cap)
			return 0;
		break;
	case V4L2_BUF_TYPE_META_OUTPUT:
		if (is_meta && is_tx && ops->vidioc_g_fmt_meta_out)
			return 0;
		break;
	default:
		break;
	}
	return -EINVAL;
}