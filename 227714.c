static void uvc_stream_delete(struct uvc_streaming *stream)
{
	if (stream->async_wq)
		destroy_workqueue(stream->async_wq);

	mutex_destroy(&stream->mutex);

	usb_put_intf(stream->intf);

	kfree(stream->format);
	kfree(stream->header.bmaControls);
	kfree(stream);
}