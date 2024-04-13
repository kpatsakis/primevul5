setup_rx_reqs(struct printer_dev *dev)
{
	struct usb_request              *req;

	while (likely(!list_empty(&dev->rx_reqs))) {
		int error;

		req = container_of(dev->rx_reqs.next,
				struct usb_request, list);
		list_del_init(&req->list);

		/* The USB Host sends us whatever amount of data it wants to
		 * so we always set the length field to the full USB_BUFSIZE.
		 * If the amount of data is more than the read() caller asked
		 * for it will be stored in the request buffer until it is
		 * asked for by read().
		 */
		req->length = USB_BUFSIZE;
		req->complete = rx_complete;

		/* here, we unlock, and only unlock, to avoid deadlock. */
		spin_unlock(&dev->lock);
		error = usb_ep_queue(dev->out_ep, req, GFP_ATOMIC);
		spin_lock(&dev->lock);
		if (error) {
			DBG(dev, "rx submit --> %d\n", error);
			list_add(&req->list, &dev->rx_reqs);
			break;
		}
		/* if the req is empty, then add it into dev->rx_reqs_active. */
		else if (list_empty(&req->list))
			list_add(&req->list, &dev->rx_reqs_active);
	}
}