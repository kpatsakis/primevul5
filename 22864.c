int config_ep_by_speed_and_alt(struct usb_gadget *g,
				struct usb_function *f,
				struct usb_ep *_ep,
				u8 alt)
{
	struct usb_endpoint_descriptor *chosen_desc = NULL;
	struct usb_interface_descriptor *int_desc = NULL;
	struct usb_descriptor_header **speed_desc = NULL;

	struct usb_ss_ep_comp_descriptor *comp_desc = NULL;
	int want_comp_desc = 0;

	struct usb_descriptor_header **d_spd; /* cursor for speed desc */
	struct usb_composite_dev *cdev;
	bool incomplete_desc = false;

	if (!g || !f || !_ep)
		return -EIO;

	/* select desired speed */
	switch (g->speed) {
	case USB_SPEED_SUPER_PLUS:
		if (gadget_is_superspeed_plus(g)) {
			if (f->ssp_descriptors) {
				speed_desc = f->ssp_descriptors;
				want_comp_desc = 1;
				break;
			}
			incomplete_desc = true;
		}
		fallthrough;
	case USB_SPEED_SUPER:
		if (gadget_is_superspeed(g)) {
			if (f->ss_descriptors) {
				speed_desc = f->ss_descriptors;
				want_comp_desc = 1;
				break;
			}
			incomplete_desc = true;
		}
		fallthrough;
	case USB_SPEED_HIGH:
		if (gadget_is_dualspeed(g)) {
			if (f->hs_descriptors) {
				speed_desc = f->hs_descriptors;
				break;
			}
			incomplete_desc = true;
		}
		fallthrough;
	default:
		speed_desc = f->fs_descriptors;
	}

	cdev = get_gadget_data(g);
	if (incomplete_desc)
		WARNING(cdev,
			"%s doesn't hold the descriptors for current speed\n",
			f->name);

	/* find correct alternate setting descriptor */
	for_each_desc(speed_desc, d_spd, USB_DT_INTERFACE) {
		int_desc = (struct usb_interface_descriptor *)*d_spd;

		if (int_desc->bAlternateSetting == alt) {
			speed_desc = d_spd;
			goto intf_found;
		}
	}
	return -EIO;

intf_found:
	/* find descriptors */
	for_each_desc(speed_desc, d_spd, USB_DT_ENDPOINT) {
		chosen_desc = (struct usb_endpoint_descriptor *)*d_spd;
		if (chosen_desc->bEndpointAddress == _ep->address)
			goto ep_found;
	}
	return -EIO;

ep_found:
	/* commit results */
	_ep->maxpacket = usb_endpoint_maxp(chosen_desc);
	_ep->desc = chosen_desc;
	_ep->comp_desc = NULL;
	_ep->maxburst = 0;
	_ep->mult = 1;

	if (g->speed == USB_SPEED_HIGH && (usb_endpoint_xfer_isoc(_ep->desc) ||
				usb_endpoint_xfer_int(_ep->desc)))
		_ep->mult = usb_endpoint_maxp_mult(_ep->desc);

	if (!want_comp_desc)
		return 0;

	/*
	 * Companion descriptor should follow EP descriptor
	 * USB 3.0 spec, #9.6.7
	 */
	comp_desc = (struct usb_ss_ep_comp_descriptor *)*(++d_spd);
	if (!comp_desc ||
	    (comp_desc->bDescriptorType != USB_DT_SS_ENDPOINT_COMP))
		return -EIO;
	_ep->comp_desc = comp_desc;
	if (g->speed >= USB_SPEED_SUPER) {
		switch (usb_endpoint_type(_ep->desc)) {
		case USB_ENDPOINT_XFER_ISOC:
			/* mult: bits 1:0 of bmAttributes */
			_ep->mult = (comp_desc->bmAttributes & 0x3) + 1;
			fallthrough;
		case USB_ENDPOINT_XFER_BULK:
		case USB_ENDPOINT_XFER_INT:
			_ep->maxburst = comp_desc->bMaxBurst + 1;
			break;
		default:
			if (comp_desc->bMaxBurst != 0)
				ERROR(cdev, "ep0 bMaxBurst must be 0\n");
			_ep->maxburst = 1;
			break;
		}
	}
	return 0;
}