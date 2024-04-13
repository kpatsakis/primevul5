static void uvc_unregister_video(struct uvc_device *dev)
{
	struct uvc_streaming *stream;

	list_for_each_entry(stream, &dev->streams, list) {
		if (!video_is_registered(&stream->vdev))
			continue;

		video_unregister_device(&stream->vdev);
		video_unregister_device(&stream->meta.vdev);

		uvc_debugfs_cleanup_stream(stream);
	}

	uvc_status_unregister(dev);

	if (dev->vdev.dev)
		v4l2_device_unregister(&dev->vdev);
#ifdef CONFIG_MEDIA_CONTROLLER
	if (media_devnode_is_registered(dev->mdev.devnode))
		media_device_unregister(&dev->mdev);
#endif
}