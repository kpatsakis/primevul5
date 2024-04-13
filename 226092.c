static int printer_func_set_alt(struct usb_function *f,
		unsigned intf, unsigned alt)
{
	struct printer_dev *dev = func_to_printer(f);
	int ret = -ENOTSUPP;

	if (!alt)
		ret = set_interface(dev, intf);

	return ret;
}