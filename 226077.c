static int printer_func_setup(struct usb_function *f,
		const struct usb_ctrlrequest *ctrl)
{
	struct printer_dev *dev = func_to_printer(f);
	struct usb_composite_dev *cdev = f->config->cdev;
	struct usb_request	*req = cdev->req;
	u8			*buf = req->buf;
	int			value = -EOPNOTSUPP;
	u16			wIndex = le16_to_cpu(ctrl->wIndex);
	u16			wValue = le16_to_cpu(ctrl->wValue);
	u16			wLength = le16_to_cpu(ctrl->wLength);

	DBG(dev, "ctrl req%02x.%02x v%04x i%04x l%d\n",
		ctrl->bRequestType, ctrl->bRequest, wValue, wIndex, wLength);

	switch (ctrl->bRequestType&USB_TYPE_MASK) {
	case USB_TYPE_CLASS:
		switch (ctrl->bRequest) {
		case GET_DEVICE_ID: /* Get the IEEE-1284 PNP String */
			/* Only one printer interface is supported. */
			if ((wIndex>>8) != dev->interface)
				break;

			if (!dev->pnp_string) {
				value = 0;
				break;
			}
			value = strlen(dev->pnp_string);
			buf[0] = (value >> 8) & 0xFF;
			buf[1] = value & 0xFF;
			memcpy(buf + 2, dev->pnp_string, value);
			DBG(dev, "1284 PNP String: %x %s\n", value,
			    dev->pnp_string);
			break;

		case GET_PORT_STATUS: /* Get Port Status */
			/* Only one printer interface is supported. */
			if (wIndex != dev->interface)
				break;

			buf[0] = dev->printer_status;
			value = min_t(u16, wLength, 1);
			break;

		case SOFT_RESET: /* Soft Reset */
			/* Only one printer interface is supported. */
			if (wIndex != dev->interface)
				break;

			printer_soft_reset(dev);

			value = 0;
			break;

		default:
			goto unknown;
		}
		break;

	default:
unknown:
		VDBG(dev,
			"unknown ctrl req%02x.%02x v%04x i%04x l%d\n",
			ctrl->bRequestType, ctrl->bRequest,
			wValue, wIndex, wLength);
		break;
	}
	/* host either stalls (value < 0) or reports success */
	if (value >= 0) {
		req->length = value;
		req->zero = value < wLength;
		value = usb_ep_queue(cdev->gadget->ep0, req, GFP_ATOMIC);
		if (value < 0) {
			ERROR(dev, "%s:%d Error!\n", __func__, __LINE__);
			req->status = 0;
		}
	}
	return value;
}