static u8 encode_bMaxPower(enum usb_device_speed speed,
		struct usb_configuration *c)
{
	unsigned val;

	if (c->MaxPower || (c->bmAttributes & USB_CONFIG_ATT_SELFPOWER))
		val = c->MaxPower;
	else
		val = CONFIG_USB_GADGET_VBUS_DRAW;
	if (!val)
		return 0;
	if (speed < USB_SPEED_SUPER)
		return min(val, 500U) / 2;
	else
		/*
		 * USB 3.x supports up to 900mA, but since 900 isn't divisible
		 * by 8 the integral division will effectively cap to 896mA.
		 */
		return min(val, 900U) / 8;
}