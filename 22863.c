void composite_resume(struct usb_gadget *gadget)
{
	struct usb_composite_dev	*cdev = get_gadget_data(gadget);
	struct usb_function		*f;
	unsigned			maxpower;

	/* REVISIT:  should we have config level
	 * suspend/resume callbacks?
	 */
	DBG(cdev, "resume\n");
	if (cdev->driver->resume)
		cdev->driver->resume(cdev);
	if (cdev->config) {
		list_for_each_entry(f, &cdev->config->functions, list) {
			if (f->resume)
				f->resume(f);
		}

		maxpower = cdev->config->MaxPower ?
			cdev->config->MaxPower : CONFIG_USB_GADGET_VBUS_DRAW;
		if (gadget->speed < USB_SPEED_SUPER)
			maxpower = min(maxpower, 500U);
		else
			maxpower = min(maxpower, 900U);

		if (maxpower > USB_SELF_POWER_VBUS_MAX_DRAW)
			usb_gadget_clear_selfpowered(gadget);

		usb_gadget_vbus_draw(gadget, maxpower);
	}

	cdev->suspended = 0;
}