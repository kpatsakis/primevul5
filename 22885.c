static int count_ext_prop(struct usb_configuration *c, int interface)
{
	struct usb_function *f;
	int j;

	f = c->interface[interface];
	for (j = 0; j < f->os_desc_n; ++j) {
		struct usb_os_desc *d;

		if (interface != f->os_desc_table[j].if_id)
			continue;
		d = f->os_desc_table[j].os_desc;
		if (d && d->ext_compat_id)
			return d->ext_prop_count;
	}
	return 0;
}