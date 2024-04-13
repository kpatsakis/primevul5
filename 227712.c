static int __uvc_resume(struct usb_interface *intf, int reset)
{
	struct uvc_device *dev = usb_get_intfdata(intf);
	struct uvc_streaming *stream;
	int ret = 0;

	uvc_trace(UVC_TRACE_SUSPEND, "Resuming interface %u\n",
		intf->cur_altsetting->desc.bInterfaceNumber);

	if (intf->cur_altsetting->desc.bInterfaceSubClass ==
	    UVC_SC_VIDEOCONTROL) {
		if (reset) {
			ret = uvc_ctrl_restore_values(dev);
			if (ret < 0)
				return ret;
		}

		mutex_lock(&dev->lock);
		if (dev->users)
			ret = uvc_status_start(dev, GFP_NOIO);
		mutex_unlock(&dev->lock);

		return ret;
	}

	list_for_each_entry(stream, &dev->streams, list) {
		if (stream->intf == intf) {
			ret = uvc_video_resume(stream, reset);
			if (ret < 0)
				uvc_queue_streamoff(&stream->queue,
						    stream->queue.queue.type);
			return ret;
		}
	}

	uvc_trace(UVC_TRACE_SUSPEND, "Resume: video streaming USB interface "
			"mismatch.\n");
	return -EINVAL;
}