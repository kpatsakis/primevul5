static int set_config(struct usb_composite_dev *cdev,
		const struct usb_ctrlrequest *ctrl, unsigned number)
{
	struct usb_gadget	*gadget = cdev->gadget;
	struct usb_configuration *c = NULL;
	int			result = -EINVAL;
	unsigned		power = gadget_is_otg(gadget) ? 8 : 100;
	int			tmp;

	if (number) {
		list_for_each_entry(c, &cdev->configs, list) {
			if (c->bConfigurationValue == number) {
				/*
				 * We disable the FDs of the previous
				 * configuration only if the new configuration
				 * is a valid one
				 */
				if (cdev->config)
					reset_config(cdev);
				result = 0;
				break;
			}
		}
		if (result < 0)
			goto done;
	} else { /* Zero configuration value - need to reset the config */
		if (cdev->config)
			reset_config(cdev);
		result = 0;
	}

	DBG(cdev, "%s config #%d: %s\n",
	    usb_speed_string(gadget->speed),
	    number, c ? c->label : "unconfigured");

	if (!c)
		goto done;

	usb_gadget_set_state(gadget, USB_STATE_CONFIGURED);
	cdev->config = c;

	/* Initialize all interfaces by setting them to altsetting zero. */
	for (tmp = 0; tmp < MAX_CONFIG_INTERFACES; tmp++) {
		struct usb_function	*f = c->interface[tmp];
		struct usb_descriptor_header **descriptors;

		if (!f)
			break;

		/*
		 * Record which endpoints are used by the function. This is used
		 * to dispatch control requests targeted at that endpoint to the
		 * function's setup callback instead of the current
		 * configuration's setup callback.
		 */
		descriptors = function_descriptors(f, gadget->speed);

		for (; *descriptors; ++descriptors) {
			struct usb_endpoint_descriptor *ep;
			int addr;

			if ((*descriptors)->bDescriptorType != USB_DT_ENDPOINT)
				continue;

			ep = (struct usb_endpoint_descriptor *)*descriptors;
			addr = ((ep->bEndpointAddress & 0x80) >> 3)
			     |  (ep->bEndpointAddress & 0x0f);
			set_bit(addr, f->endpoints);
		}

		result = f->set_alt(f, tmp, 0);
		if (result < 0) {
			DBG(cdev, "interface %d (%s/%p) alt 0 --> %d\n",
					tmp, f->name, f, result);

			reset_config(cdev);
			goto done;
		}

		if (result == USB_GADGET_DELAYED_STATUS) {
			DBG(cdev,
			 "%s: interface %d (%s) requested delayed status\n",
					__func__, tmp, f->name);
			cdev->delayed_status++;
			DBG(cdev, "delayed_status count %d\n",
					cdev->delayed_status);
		}
	}

	/* when we return, be sure our power usage is valid */
	if (c->MaxPower || (c->bmAttributes & USB_CONFIG_ATT_SELFPOWER))
		power = c->MaxPower;
	else
		power = CONFIG_USB_GADGET_VBUS_DRAW;

	if (gadget->speed < USB_SPEED_SUPER)
		power = min(power, 500U);
	else
		power = min(power, 900U);
done:
	if (power <= USB_SELF_POWER_VBUS_MAX_DRAW)
		usb_gadget_set_selfpowered(gadget);
	else
		usb_gadget_clear_selfpowered(gadget);

	usb_gadget_vbus_draw(gadget, power);
	if (result >= 0 && cdev->delayed_status)
		result = USB_GADGET_DELAYED_STATUS;
	return result;
}