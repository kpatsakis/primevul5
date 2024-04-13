static void __composite_unbind(struct usb_gadget *gadget, bool unbind_driver)
{
	struct usb_composite_dev	*cdev = get_gadget_data(gadget);
	struct usb_gadget_strings	*gstr = cdev->driver->strings[0];
	struct usb_string		*dev_str = gstr->strings;

	/* composite_disconnect() must already have been called
	 * by the underlying peripheral controller driver!
	 * so there's no i/o concurrency that could affect the
	 * state protected by cdev->lock.
	 */
	WARN_ON(cdev->config);

	while (!list_empty(&cdev->configs)) {
		struct usb_configuration	*c;
		c = list_first_entry(&cdev->configs,
				struct usb_configuration, list);
		remove_config(cdev, c);
	}
	if (cdev->driver->unbind && unbind_driver)
		cdev->driver->unbind(cdev);

	composite_dev_cleanup(cdev);

	if (dev_str[USB_GADGET_MANUFACTURER_IDX].s == cdev->def_manufacturer)
		dev_str[USB_GADGET_MANUFACTURER_IDX].s = "";

	kfree(cdev->def_manufacturer);
	kfree(cdev);
	set_gadget_data(gadget, NULL);
}