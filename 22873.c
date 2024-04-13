static int config_desc(struct usb_composite_dev *cdev, unsigned w_value)
{
	struct usb_gadget		*gadget = cdev->gadget;
	struct usb_configuration	*c;
	struct list_head		*pos;
	u8				type = w_value >> 8;
	enum usb_device_speed		speed = USB_SPEED_UNKNOWN;

	if (gadget->speed >= USB_SPEED_SUPER)
		speed = gadget->speed;
	else if (gadget_is_dualspeed(gadget)) {
		int	hs = 0;
		if (gadget->speed == USB_SPEED_HIGH)
			hs = 1;
		if (type == USB_DT_OTHER_SPEED_CONFIG)
			hs = !hs;
		if (hs)
			speed = USB_SPEED_HIGH;

	}

	/* This is a lookup by config *INDEX* */
	w_value &= 0xff;

	pos = &cdev->configs;
	c = cdev->os_desc_config;
	if (c)
		goto check_config;

	while ((pos = pos->next) !=  &cdev->configs) {
		c = list_entry(pos, typeof(*c), list);

		/* skip OS Descriptors config which is handled separately */
		if (c == cdev->os_desc_config)
			continue;

check_config:
		/* ignore configs that won't work at this speed */
		switch (speed) {
		case USB_SPEED_SUPER_PLUS:
			if (!c->superspeed_plus)
				continue;
			break;
		case USB_SPEED_SUPER:
			if (!c->superspeed)
				continue;
			break;
		case USB_SPEED_HIGH:
			if (!c->highspeed)
				continue;
			break;
		default:
			if (!c->fullspeed)
				continue;
		}

		if (w_value == 0)
			return config_buf(c, speed, cdev->req->buf, type);
		w_value--;
	}
	return -EINVAL;
}