static int uvc_resume(struct usb_interface *intf)
{
	return __uvc_resume(intf, 0);
}