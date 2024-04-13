static int len_ext_prop(struct usb_configuration *c, int interface)
{
	struct usb_function *f;
	struct usb_os_desc *d;
	int j, res;

	res = 10; /* header length */
	f = c->interface[interface];
	for (j = 0; j < f->os_desc_n; ++j) {
		if (interface != f->os_desc_table[j].if_id)
			continue;
		d = f->os_desc_table[j].os_desc;
		if (d)
			return min(res + d->ext_prop_len, 4096);
	}
	return res;
}