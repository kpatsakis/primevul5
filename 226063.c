static void printer_func_disable(struct usb_function *f)
{
	struct printer_dev *dev = func_to_printer(f);

	DBG(dev, "%s\n", __func__);

	printer_reset_interface(dev);
}