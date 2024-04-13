void composite_disconnect(struct usb_gadget *gadget)
{
	usb_gadget_vbus_draw(gadget, 0);
	__composite_disconnect(gadget);
}