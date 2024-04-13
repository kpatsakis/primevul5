static int fill_ext_prop(struct usb_configuration *c, int interface, u8 *buf)
{
	struct usb_function *f;
	struct usb_os_desc *d;
	struct usb_os_desc_ext_prop *ext_prop;
	int j, count, n, ret;

	f = c->interface[interface];
	count = 10; /* header length */
	buf += 10;
	for (j = 0; j < f->os_desc_n; ++j) {
		if (interface != f->os_desc_table[j].if_id)
			continue;
		d = f->os_desc_table[j].os_desc;
		if (d)
			list_for_each_entry(ext_prop, &d->ext_prop, entry) {
				n = ext_prop->data_len +
					ext_prop->name_len + 14;
				if (count + n >= USB_COMP_EP0_OS_DESC_BUFSIZ)
					return count;
				usb_ext_prop_put_size(buf, n);
				usb_ext_prop_put_type(buf, ext_prop->type);
				ret = usb_ext_prop_put_name(buf, ext_prop->name,
							    ext_prop->name_len);
				if (ret < 0)
					return ret;
				switch (ext_prop->type) {
				case USB_EXT_PROP_UNICODE:
				case USB_EXT_PROP_UNICODE_ENV:
				case USB_EXT_PROP_UNICODE_LINK:
					usb_ext_prop_put_unicode(buf, ret,
							 ext_prop->data,
							 ext_prop->data_len);
					break;
				case USB_EXT_PROP_BINARY:
					usb_ext_prop_put_binary(buf, ret,
							ext_prop->data,
							ext_prop->data_len);
					break;
				case USB_EXT_PROP_LE32:
					/* not implemented */
				case USB_EXT_PROP_BE32:
					/* not implemented */
				default:
					return -EINVAL;
				}
				buf += n;
				count += n;
			}
	}

	return count;
}