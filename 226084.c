static void printer_func_unbind(struct usb_configuration *c,
		struct usb_function *f)
{
	struct printer_dev	*dev;
	struct usb_request	*req;

	dev = func_to_printer(f);

	device_destroy(usb_gadget_class, MKDEV(major, dev->minor));

	/* Remove Character Device */
	cdev_del(&dev->printer_cdev);

	/* we must already have been disconnected ... no i/o may be active */
	WARN_ON(!list_empty(&dev->tx_reqs_active));
	WARN_ON(!list_empty(&dev->rx_reqs_active));

	/* Free all memory for this driver. */
	while (!list_empty(&dev->tx_reqs)) {
		req = container_of(dev->tx_reqs.next, struct usb_request,
				list);
		list_del(&req->list);
		printer_req_free(dev->in_ep, req);
	}

	if (dev->current_rx_req != NULL)
		printer_req_free(dev->out_ep, dev->current_rx_req);

	while (!list_empty(&dev->rx_reqs)) {
		req = container_of(dev->rx_reqs.next,
				struct usb_request, list);
		list_del(&req->list);
		printer_req_free(dev->out_ep, req);
	}

	while (!list_empty(&dev->rx_buffers)) {
		req = container_of(dev->rx_buffers.next,
				struct usb_request, list);
		list_del(&req->list);
		printer_req_free(dev->out_ep, req);
	}
	usb_free_all_descriptors(f);
}