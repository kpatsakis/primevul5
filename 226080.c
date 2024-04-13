static bool gprinter_req_match(struct usb_function *f,
			       const struct usb_ctrlrequest *ctrl,
			       bool config0)
{
	struct printer_dev	*dev = func_to_printer(f);
	u16			w_index = le16_to_cpu(ctrl->wIndex);
	u16			w_value = le16_to_cpu(ctrl->wValue);
	u16			w_length = le16_to_cpu(ctrl->wLength);

	if (config0)
		return false;

	if ((ctrl->bRequestType & USB_RECIP_MASK) != USB_RECIP_INTERFACE ||
	    (ctrl->bRequestType & USB_TYPE_MASK) != USB_TYPE_CLASS)
		return false;

	switch (ctrl->bRequest) {
	case GET_DEVICE_ID:
		w_index >>= 8;
		if (USB_DIR_IN & ctrl->bRequestType)
			break;
		return false;
	case GET_PORT_STATUS:
		if (!w_value && w_length == 1 &&
		    (USB_DIR_IN & ctrl->bRequestType))
			break;
		return false;
	case SOFT_RESET:
		if (!w_value && !w_length &&
		   !(USB_DIR_IN & ctrl->bRequestType))
			break;
		fallthrough;
	default:
		return false;
	}
	return w_index == dev->interface;
}