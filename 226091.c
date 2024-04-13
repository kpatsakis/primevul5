static inline struct printer_dev *func_to_printer(struct usb_function *f)
{
	return container_of(f, struct printer_dev, function);
}