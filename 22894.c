static int count_ext_compat(struct usb_configuration *c)
{
	int i, res;

	res = 0;
	for (i = 0; i < c->next_interface_id; ++i) {
		struct usb_function *f;
		int j;

		f = c->interface[i];
		for (j = 0; j < f->os_desc_n; ++j) {
			struct usb_os_desc *d;

			if (i != f->os_desc_table[j].if_id)
				continue;
			d = f->os_desc_table[j].os_desc;
			if (d && d->ext_compat_id)
				++res;
		}
	}
	BUG_ON(res > 255);
	return res;
}