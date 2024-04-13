static int fill_ext_compat(struct usb_configuration *c, u8 *buf)
{
	int i, count;

	count = 16;
	buf += 16;
	for (i = 0; i < c->next_interface_id; ++i) {
		struct usb_function *f;
		int j;

		f = c->interface[i];
		for (j = 0; j < f->os_desc_n; ++j) {
			struct usb_os_desc *d;

			if (i != f->os_desc_table[j].if_id)
				continue;
			d = f->os_desc_table[j].os_desc;
			if (d && d->ext_compat_id) {
				*buf++ = i;
				*buf++ = 0x01;
				memcpy(buf, d->ext_compat_id, 16);
				buf += 22;
			} else {
				++buf;
				*buf = 0x01;
				buf += 23;
			}
			count += 24;
			if (count + 24 >= USB_COMP_EP0_OS_DESC_BUFSIZ)
				return count;
		}
	}

	return count;
}