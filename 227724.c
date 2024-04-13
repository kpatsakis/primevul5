static void uvc_disconnect(struct usb_interface *intf)
{
	struct uvc_device *dev = usb_get_intfdata(intf);

	/* Set the USB interface data to NULL. This can be done outside the
	 * lock, as there's no other reader.
	 */
	usb_set_intfdata(intf, NULL);

	if (intf->cur_altsetting->desc.bInterfaceSubClass ==
	    UVC_SC_VIDEOSTREAMING)
		return;

	uvc_unregister_video(dev);
	kref_put(&dev->ref, uvc_delete);
}