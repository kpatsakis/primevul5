void composite_reset(struct usb_gadget *gadget)
{
	/*
	 * Section 1.4.13 Standard Downstream Port of the USB battery charging
	 * specification v1.2 states that a device connected on a SDP shall only
	 * draw at max 100mA while in a connected, but unconfigured state.
	 */
	usb_gadget_vbus_draw(gadget, 100);
	__composite_disconnect(gadget);
}