static int uvc_register_video(struct uvc_device *dev,
		struct uvc_streaming *stream)
{
	int ret;

	/* Initialize the streaming interface with default parameters. */
	ret = uvc_video_init(stream);
	if (ret < 0) {
		uvc_printk(KERN_ERR, "Failed to initialize the device (%d).\n",
			   ret);
		return ret;
	}

	if (stream->type == V4L2_BUF_TYPE_VIDEO_CAPTURE)
		stream->chain->caps |= V4L2_CAP_VIDEO_CAPTURE
			| V4L2_CAP_META_CAPTURE;
	else
		stream->chain->caps |= V4L2_CAP_VIDEO_OUTPUT;

	uvc_debugfs_init_stream(stream);

	/* Register the device with V4L. */
	return uvc_register_video_device(dev, stream, &stream->vdev,
					 &stream->queue, stream->type,
					 &uvc_fops, &uvc_ioctl_ops);
}