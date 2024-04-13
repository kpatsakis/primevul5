static int printer_func_bind(struct usb_configuration *c,
		struct usb_function *f)
{
	struct usb_gadget *gadget = c->cdev->gadget;
	struct printer_dev *dev = func_to_printer(f);
	struct device *pdev;
	struct usb_composite_dev *cdev = c->cdev;
	struct usb_ep *in_ep;
	struct usb_ep *out_ep = NULL;
	struct usb_request *req;
	dev_t devt;
	int id;
	int ret;
	u32 i;

	id = usb_interface_id(c, f);
	if (id < 0)
		return id;
	intf_desc.bInterfaceNumber = id;

	/* finish hookup to lower layer ... */
	dev->gadget = gadget;

	/* all we really need is bulk IN/OUT */
	in_ep = usb_ep_autoconfig(cdev->gadget, &fs_ep_in_desc);
	if (!in_ep) {
autoconf_fail:
		dev_err(&cdev->gadget->dev, "can't autoconfigure on %s\n",
			cdev->gadget->name);
		return -ENODEV;
	}

	out_ep = usb_ep_autoconfig(cdev->gadget, &fs_ep_out_desc);
	if (!out_ep)
		goto autoconf_fail;

	/* assumes that all endpoints are dual-speed */
	hs_ep_in_desc.bEndpointAddress = fs_ep_in_desc.bEndpointAddress;
	hs_ep_out_desc.bEndpointAddress = fs_ep_out_desc.bEndpointAddress;
	ss_ep_in_desc.bEndpointAddress = fs_ep_in_desc.bEndpointAddress;
	ss_ep_out_desc.bEndpointAddress = fs_ep_out_desc.bEndpointAddress;

	ret = usb_assign_descriptors(f, fs_printer_function,
			hs_printer_function, ss_printer_function, NULL);
	if (ret)
		return ret;

	dev->in_ep = in_ep;
	dev->out_ep = out_ep;

	ret = -ENOMEM;
	for (i = 0; i < dev->q_len; i++) {
		req = printer_req_alloc(dev->in_ep, USB_BUFSIZE, GFP_KERNEL);
		if (!req)
			goto fail_tx_reqs;
		list_add(&req->list, &dev->tx_reqs);
	}

	for (i = 0; i < dev->q_len; i++) {
		req = printer_req_alloc(dev->out_ep, USB_BUFSIZE, GFP_KERNEL);
		if (!req)
			goto fail_rx_reqs;
		list_add(&req->list, &dev->rx_reqs);
	}

	/* Setup the sysfs files for the printer gadget. */
	devt = MKDEV(major, dev->minor);
	pdev = device_create(usb_gadget_class, NULL, devt,
				  NULL, "g_printer%d", dev->minor);
	if (IS_ERR(pdev)) {
		ERROR(dev, "Failed to create device: g_printer\n");
		ret = PTR_ERR(pdev);
		goto fail_rx_reqs;
	}

	/*
	 * Register a character device as an interface to a user mode
	 * program that handles the printer specific functionality.
	 */
	cdev_init(&dev->printer_cdev, &printer_io_operations);
	dev->printer_cdev.owner = THIS_MODULE;
	ret = cdev_add(&dev->printer_cdev, devt, 1);
	if (ret) {
		ERROR(dev, "Failed to open char device\n");
		goto fail_cdev_add;
	}

	return 0;

fail_cdev_add:
	device_destroy(usb_gadget_class, devt);

fail_rx_reqs:
	while (!list_empty(&dev->rx_reqs)) {
		req = container_of(dev->rx_reqs.next, struct usb_request, list);
		list_del(&req->list);
		printer_req_free(dev->out_ep, req);
	}

fail_tx_reqs:
	while (!list_empty(&dev->tx_reqs)) {
		req = container_of(dev->tx_reqs.next, struct usb_request, list);
		list_del(&req->list);
		printer_req_free(dev->in_ep, req);
	}

	return ret;

}