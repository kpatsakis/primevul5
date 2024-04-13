int uvc_register_video_device(struct uvc_device *dev,
			      struct uvc_streaming *stream,
			      struct video_device *vdev,
			      struct uvc_video_queue *queue,
			      enum v4l2_buf_type type,
			      const struct v4l2_file_operations *fops,
			      const struct v4l2_ioctl_ops *ioctl_ops)
{
	int ret;

	/* Initialize the video buffers queue. */
	ret = uvc_queue_init(queue, type, !uvc_no_drop_param);
	if (ret)
		return ret;

	/* Register the device with V4L. */

	/*
	 * We already hold a reference to dev->udev. The video device will be
	 * unregistered before the reference is released, so we don't need to
	 * get another one.
	 */
	vdev->v4l2_dev = &dev->vdev;
	vdev->fops = fops;
	vdev->ioctl_ops = ioctl_ops;
	vdev->release = uvc_release;
	vdev->prio = &stream->chain->prio;
	if (type == V4L2_BUF_TYPE_VIDEO_OUTPUT)
		vdev->vfl_dir = VFL_DIR_TX;
	else
		vdev->vfl_dir = VFL_DIR_RX;

	switch (type) {
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
	default:
		vdev->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
		break;
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
		vdev->device_caps = V4L2_CAP_VIDEO_OUTPUT | V4L2_CAP_STREAMING;
		break;
	case V4L2_BUF_TYPE_META_CAPTURE:
		vdev->device_caps = V4L2_CAP_META_CAPTURE | V4L2_CAP_STREAMING;
		break;
	}

	strscpy(vdev->name, dev->name, sizeof(vdev->name));

	/*
	 * Set the driver data before calling video_register_device, otherwise
	 * the file open() handler might race us.
	 */
	video_set_drvdata(vdev, stream);

	ret = video_register_device(vdev, VFL_TYPE_GRABBER, -1);
	if (ret < 0) {
		uvc_printk(KERN_ERR, "Failed to register %s device (%d).\n",
			   v4l2_type_names[type], ret);
		return ret;
	}

	kref_get(&dev->ref);
	return 0;
}