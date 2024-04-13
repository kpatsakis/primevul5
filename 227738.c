static void uvc_release(struct video_device *vdev)
{
	struct uvc_streaming *stream = video_get_drvdata(vdev);
	struct uvc_device *dev = stream->dev;

	kref_put(&dev->ref, uvc_delete);
}