static int get_string(struct usb_composite_dev *cdev,
		void *buf, u16 language, int id)
{
	struct usb_composite_driver	*composite = cdev->driver;
	struct usb_gadget_string_container *uc;
	struct usb_configuration	*c;
	struct usb_function		*f;
	int				len;

	/* Yes, not only is USB's i18n support probably more than most
	 * folk will ever care about ... also, it's all supported here.
	 * (Except for UTF8 support for Unicode's "Astral Planes".)
	 */

	/* 0 == report all available language codes */
	if (id == 0) {
		struct usb_string_descriptor	*s = buf;
		struct usb_gadget_strings	**sp;

		memset(s, 0, 256);
		s->bDescriptorType = USB_DT_STRING;

		sp = composite->strings;
		if (sp)
			collect_langs(sp, s->wData);

		list_for_each_entry(c, &cdev->configs, list) {
			sp = c->strings;
			if (sp)
				collect_langs(sp, s->wData);

			list_for_each_entry(f, &c->functions, list) {
				sp = f->strings;
				if (sp)
					collect_langs(sp, s->wData);
			}
		}
		list_for_each_entry(uc, &cdev->gstrings, list) {
			struct usb_gadget_strings **sp;

			sp = get_containers_gs(uc);
			collect_langs(sp, s->wData);
		}

		for (len = 0; len <= USB_MAX_STRING_LEN && s->wData[len]; len++)
			continue;
		if (!len)
			return -EINVAL;

		s->bLength = 2 * (len + 1);
		return s->bLength;
	}

	if (cdev->use_os_string && language == 0 && id == OS_STRING_IDX) {
		struct usb_os_string *b = buf;
		b->bLength = sizeof(*b);
		b->bDescriptorType = USB_DT_STRING;
		compiletime_assert(
			sizeof(b->qwSignature) == sizeof(cdev->qw_sign),
			"qwSignature size must be equal to qw_sign");
		memcpy(&b->qwSignature, cdev->qw_sign, sizeof(b->qwSignature));
		b->bMS_VendorCode = cdev->b_vendor_code;
		b->bPad = 0;
		return sizeof(*b);
	}

	list_for_each_entry(uc, &cdev->gstrings, list) {
		struct usb_gadget_strings **sp;

		sp = get_containers_gs(uc);
		len = lookup_string(sp, buf, language, id);
		if (len > 0)
			return len;
	}

	/* String IDs are device-scoped, so we look up each string
	 * table we're told about.  These lookups are infrequent;
	 * simpler-is-better here.
	 */
	if (composite->strings) {
		len = lookup_string(composite->strings, buf, language, id);
		if (len > 0)
			return len;
	}
	list_for_each_entry(c, &cdev->configs, list) {
		if (c->strings) {
			len = lookup_string(c->strings, buf, language, id);
			if (len > 0)
				return len;
		}
		list_for_each_entry(f, &c->functions, list) {
			if (!f->strings)
				continue;
			len = lookup_string(f->strings, buf, language, id);
			if (len > 0)
				return len;
		}
	}
	return -EINVAL;
}