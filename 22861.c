int config_ep_by_speed(struct usb_gadget *g,
			struct usb_function *f,
			struct usb_ep *_ep)
{
	return config_ep_by_speed_and_alt(g, f, _ep, 0);
}