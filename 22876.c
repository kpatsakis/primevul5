function_descriptors(struct usb_function *f,
		     enum usb_device_speed speed)
{
	struct usb_descriptor_header **descriptors;

	/*
	 * NOTE: we try to help gadget drivers which might not be setting
	 * max_speed appropriately.
	 */

	switch (speed) {
	case USB_SPEED_SUPER_PLUS:
		descriptors = f->ssp_descriptors;
		if (descriptors)
			break;
		fallthrough;
	case USB_SPEED_SUPER:
		descriptors = f->ss_descriptors;
		if (descriptors)
			break;
		fallthrough;
	case USB_SPEED_HIGH:
		descriptors = f->hs_descriptors;
		if (descriptors)
			break;
		fallthrough;
	default:
		descriptors = f->fs_descriptors;
	}

	/*
	 * if we can't find any descriptors at all, then this gadget deserves to
	 * Oops with a NULL pointer dereference
	 */

	return descriptors;
}