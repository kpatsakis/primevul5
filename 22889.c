void composite_dev_cleanup(struct usb_composite_dev *cdev)
{
	struct usb_gadget_string_container *uc, *tmp;
	struct usb_ep			   *ep, *tmp_ep;

	list_for_each_entry_safe(uc, tmp, &cdev->gstrings, list) {
		list_del(&uc->list);
		kfree(uc);
	}
	if (cdev->os_desc_req) {
		if (cdev->os_desc_pending)
			usb_ep_dequeue(cdev->gadget->ep0, cdev->os_desc_req);

		kfree(cdev->os_desc_req->buf);
		cdev->os_desc_req->buf = NULL;
		usb_ep_free_request(cdev->gadget->ep0, cdev->os_desc_req);
		cdev->os_desc_req = NULL;
	}
	if (cdev->req) {
		if (cdev->setup_pending)
			usb_ep_dequeue(cdev->gadget->ep0, cdev->req);

		kfree(cdev->req->buf);
		cdev->req->buf = NULL;
		usb_ep_free_request(cdev->gadget->ep0, cdev->req);
		cdev->req = NULL;
	}
	cdev->next_string_id = 0;
	device_remove_file(&cdev->gadget->dev, &dev_attr_suspended);

	/*
	 * Some UDC backends have a dynamic EP allocation scheme.
	 *
	 * In that case, the dispose() callback is used to notify the
	 * backend that the EPs are no longer in use.
	 *
	 * Note: The UDC backend can remove the EP from the ep_list as
	 *	 a result, so we need to use the _safe list iterator.
	 */
	list_for_each_entry_safe(ep, tmp_ep,
				 &cdev->gadget->ep_list, ep_list) {
		if (ep->ops->dispose)
			ep->ops->dispose(ep);
	}
}