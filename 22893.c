static void composite_setup_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct usb_composite_dev *cdev;

	if (req->status || req->actual != req->length)
		DBG((struct usb_composite_dev *) ep->driver_data,
				"setup complete --> %d, %d/%d\n",
				req->status, req->actual, req->length);

	/*
	 * REVIST The same ep0 requests are shared with function drivers
	 * so they don't have to maintain the same ->complete() stubs.
	 *
	 * Because of that, we need to check for the validity of ->context
	 * here, even though we know we've set it to something useful.
	 */
	if (!req->context)
		return;

	cdev = req->context;

	if (cdev->req == req)
		cdev->setup_pending = false;
	else if (cdev->os_desc_req == req)
		cdev->os_desc_pending = false;
	else
		WARN(1, "unknown request %p\n", req);
}