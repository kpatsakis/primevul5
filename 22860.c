static void composite_unbind(struct usb_gadget *gadget)
{
	__composite_unbind(gadget, true);
}