static void init_vdev(struct video_device *vdev, int nr)
{
	MARK();

#ifdef V4L2LOOPBACK_WITH_STD
	vdev->tvnorms = V4L2_STD_ALL;
#endif /* V4L2LOOPBACK_WITH_STD */

	vdev->vfl_type = VFL_TYPE_VIDEO;
	vdev->fops = &v4l2_loopback_fops;
	vdev->ioctl_ops = &v4l2_loopback_ioctl_ops;
	vdev->release = &video_device_release;
	vdev->minor = -1;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 7, 0)
	vdev->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_VIDEO_OUTPUT |
			    V4L2_CAP_READWRITE | V4L2_CAP_STREAMING;
#ifdef V4L2_CAP_VIDEO_M2M
	vdev->device_caps |= V4L2_CAP_VIDEO_M2M;
#endif
#endif /* >=linux-4.7.0 */

	if (debug > 1)
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 20, 0)
		vdev->debug = V4L2_DEBUG_IOCTL | V4L2_DEBUG_IOCTL_ARG;
#else
		vdev->dev_debug =
			V4L2_DEV_DEBUG_IOCTL | V4L2_DEV_DEBUG_IOCTL_ARG;
#endif

		/* since kernel-3.7, there is a new field 'vfl_dir' that has to be
	 * set to VFL_DIR_M2M for bidirectional devices */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 7, 0)
	vdev->vfl_dir = VFL_DIR_M2M;
#endif

	MARK();
}