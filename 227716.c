static int uvc_suspend(struct usb_interface *intf, pm_message_t message)
{
	struct uvc_device *dev = usb_get_intfdata(intf);
	struct uvc_streaming *stream;

	uvc_trace(UVC_TRACE_SUSPEND, "Suspending interface %u\n",
		intf->cur_altsetting->desc.bInterfaceNumber);

	/* Controls are cached on the fly so they don't need to be saved. */
	if (intf->cur_altsetting->desc.bInterfaceSubClass ==
	    UVC_SC_VIDEOCONTROL) {
		mutex_lock(&dev->lock);
		if (dev->users)
			uvc_status_stop(dev);
		mutex_unlock(&dev->lock);
		return 0;
	}

	list_for_each_entry(stream, &dev->streams, list) {
		if (stream->intf == intf)
			return uvc_video_suspend(stream);
	}

	uvc_trace(UVC_TRACE_SUSPEND, "Suspend: video streaming USB interface "
			"mismatch.\n");
	return -EINVAL;
}