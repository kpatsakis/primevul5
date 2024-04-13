int composite_os_desc_req_prepare(struct usb_composite_dev *cdev,
				  struct usb_ep *ep0)
{
	int ret = 0;

	cdev->os_desc_req = usb_ep_alloc_request(ep0, GFP_KERNEL);
	if (!cdev->os_desc_req) {
		ret = -ENOMEM;
		goto end;
	}

	cdev->os_desc_req->buf = kmalloc(USB_COMP_EP0_OS_DESC_BUFSIZ,
					 GFP_KERNEL);
	if (!cdev->os_desc_req->buf) {
		ret = -ENOMEM;
		usb_ep_free_request(ep0, cdev->os_desc_req);
		goto end;
	}
	cdev->os_desc_req->context = cdev;
	cdev->os_desc_req->complete = composite_setup_complete;
end:
	return ret;
}