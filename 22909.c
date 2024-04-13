int usb_composite_probe(struct usb_composite_driver *driver)
{
	struct usb_gadget_driver *gadget_driver;

	if (!driver || !driver->dev || !driver->bind)
		return -EINVAL;

	if (!driver->name)
		driver->name = "composite";

	driver->gadget_driver = composite_driver_template;
	gadget_driver = &driver->gadget_driver;

	gadget_driver->function =  (char *) driver->name;
	gadget_driver->driver.name = driver->name;
	gadget_driver->max_speed = driver->max_speed;

	return usb_gadget_probe_driver(gadget_driver);
}