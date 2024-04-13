void usb_remove_function(struct usb_configuration *c, struct usb_function *f)
{
	if (f->disable)
		f->disable(f);

	bitmap_zero(f->endpoints, 32);
	list_del(&f->list);
	if (f->unbind)
		f->unbind(c, f);

	if (f->bind_deactivated)
		usb_function_activate(f);
}