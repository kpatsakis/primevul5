static int ss_isoc_get_packet_num(struct usb_device *udev, int pipe)
{
	struct usb_host_endpoint *ep = usb_pipe_endpoint(udev, pipe);

	return USB_SS_MULT(ep->ss_ep_comp.bmAttributes)
		* (1 + ep->ss_ep_comp.bMaxBurst);
}