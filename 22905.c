static int composite_ep0_queue(struct usb_composite_dev *cdev,
		struct usb_request *req, gfp_t gfp_flags)
{
	int ret;

	ret = usb_ep_queue(cdev->gadget->ep0, req, gfp_flags);
	if (ret == 0) {
		if (cdev->req == req)
			cdev->setup_pending = true;
		else if (cdev->os_desc_req == req)
			cdev->os_desc_pending = true;
		else
			WARN(1, "unknown request %p\n", req);
	}

	return ret;
}