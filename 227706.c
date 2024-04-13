static int uvc_reset_resume(struct usb_interface *intf)
{
	return __uvc_resume(intf, 1);
}