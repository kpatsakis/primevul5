static int check_array_args(unsigned int cmd, void *parg, size_t *array_size,
			    void __user **user_ptr, void ***kernel_ptr)
{
	int ret = 0;

	switch (cmd) {
	case VIDIOC_PREPARE_BUF:
	case VIDIOC_QUERYBUF:
	case VIDIOC_QBUF:
	case VIDIOC_DQBUF: {
		struct v4l2_buffer *buf = parg;

		if (V4L2_TYPE_IS_MULTIPLANAR(buf->type) && buf->length > 0) {
			if (buf->length > VIDEO_MAX_PLANES) {
				ret = -EINVAL;
				break;
			}
			*user_ptr = (void __user *)buf->m.planes;
			*kernel_ptr = (void **)&buf->m.planes;
			*array_size = sizeof(struct v4l2_plane) * buf->length;
			ret = 1;
		}
		break;
	}

	case VIDIOC_G_EDID:
	case VIDIOC_S_EDID: {
		struct v4l2_edid *edid = parg;

		if (edid->blocks) {
			if (edid->blocks > 256) {
				ret = -EINVAL;
				break;
			}
			*user_ptr = (void __user *)edid->edid;
			*kernel_ptr = (void **)&edid->edid;
			*array_size = edid->blocks * 128;
			ret = 1;
		}
		break;
	}

	case VIDIOC_S_EXT_CTRLS:
	case VIDIOC_G_EXT_CTRLS:
	case VIDIOC_TRY_EXT_CTRLS: {
		struct v4l2_ext_controls *ctrls = parg;

		if (ctrls->count != 0) {
			if (ctrls->count > V4L2_CID_MAX_CTRLS) {
				ret = -EINVAL;
				break;
			}
			*user_ptr = (void __user *)ctrls->controls;
			*kernel_ptr = (void **)&ctrls->controls;
			*array_size = sizeof(struct v4l2_ext_control)
				    * ctrls->count;
			ret = 1;
		}
		break;
	}
	case VIDIOC_G_FMT:
	case VIDIOC_S_FMT:
	case VIDIOC_TRY_FMT: {
		struct v4l2_format *fmt = parg;

		if (fmt->type != V4L2_BUF_TYPE_VIDEO_OVERLAY &&
		    fmt->type != V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY)
			break;
		if (fmt->fmt.win.clipcount > 2048)
			return -EINVAL;
		if (!fmt->fmt.win.clipcount)
			break;

		*user_ptr = (void __user *)fmt->fmt.win.clips;
		*kernel_ptr = (void **)&fmt->fmt.win.clips;
		*array_size = sizeof(struct v4l2_clip)
				* fmt->fmt.win.clipcount;

		ret = 1;
		break;
	}
	}

	return ret;
}