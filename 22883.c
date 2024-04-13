void composite_suspend(struct usb_gadget *gadget)
{
	struct usb_composite_dev	*cdev = get_gadget_data(gadget);
	struct usb_function		*f;

	/* REVISIT:  should we have config level
	 * suspend/resume callbacks?
	 */
	DBG(cdev, "suspend\n");
	if (cdev->config) {
		list_for_each_entry(f, &cdev->config->functions, list) {
			if (f->suspend)
				f->suspend(f);
		}
	}
	if (cdev->driver->suspend)
		cdev->driver->suspend(cdev);

	cdev->suspended = 1;

	usb_gadget_set_selfpowered(gadget);
	usb_gadget_vbus_draw(gadget, 2);
}