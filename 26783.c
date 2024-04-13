static int tower_probe (struct usb_interface *interface, const struct usb_device_id *id)
{
	struct device *idev = &interface->dev;
	struct usb_device *udev = interface_to_usbdev(interface);
	struct lego_usb_tower *dev = NULL;
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor* endpoint;
	struct tower_get_version_reply get_version_reply;
	int i;
	int retval = -ENOMEM;
	int result;

	/* allocate memory for our device state and initialize it */

	dev = kmalloc (sizeof(struct lego_usb_tower), GFP_KERNEL);

	if (!dev)
		goto exit;

	mutex_init(&dev->lock);

	dev->udev = udev;
	dev->open_count = 0;

	dev->read_buffer = NULL;
	dev->read_buffer_length = 0;
	dev->read_packet_length = 0;
	spin_lock_init (&dev->read_buffer_lock);
	dev->packet_timeout_jiffies = msecs_to_jiffies(packet_timeout);
	dev->read_last_arrival = jiffies;

	init_waitqueue_head (&dev->read_wait);
	init_waitqueue_head (&dev->write_wait);

	dev->interrupt_in_buffer = NULL;
	dev->interrupt_in_endpoint = NULL;
	dev->interrupt_in_urb = NULL;
	dev->interrupt_in_running = 0;
	dev->interrupt_in_done = 0;

	dev->interrupt_out_buffer = NULL;
	dev->interrupt_out_endpoint = NULL;
	dev->interrupt_out_urb = NULL;
	dev->interrupt_out_busy = 0;

	iface_desc = interface->cur_altsetting;

	/* set up the endpoint information */
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
		endpoint = &iface_desc->endpoint[i].desc;

		if (usb_endpoint_xfer_int(endpoint)) {
			if (usb_endpoint_dir_in(endpoint))
				dev->interrupt_in_endpoint = endpoint;
			else
				dev->interrupt_out_endpoint = endpoint;
		}
	}
	if(dev->interrupt_in_endpoint == NULL) {
		dev_err(idev, "interrupt in endpoint not found\n");
		goto error;
	}
	if (dev->interrupt_out_endpoint == NULL) {
		dev_err(idev, "interrupt out endpoint not found\n");
		goto error;
	}

	dev->read_buffer = kmalloc (read_buffer_size, GFP_KERNEL);
	if (!dev->read_buffer)
		goto error;
	dev->interrupt_in_buffer = kmalloc (usb_endpoint_maxp(dev->interrupt_in_endpoint), GFP_KERNEL);
	if (!dev->interrupt_in_buffer)
		goto error;
	dev->interrupt_in_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!dev->interrupt_in_urb)
		goto error;
	dev->interrupt_out_buffer = kmalloc (write_buffer_size, GFP_KERNEL);
	if (!dev->interrupt_out_buffer)
		goto error;
	dev->interrupt_out_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!dev->interrupt_out_urb)
		goto error;
 	dev->interrupt_in_interval = interrupt_in_interval ? interrupt_in_interval : dev->interrupt_in_endpoint->bInterval;
 	dev->interrupt_out_interval = interrupt_out_interval ? interrupt_out_interval : dev->interrupt_out_endpoint->bInterval;
 
	/* we can register the device now, as it is ready */
	usb_set_intfdata (interface, dev);
	retval = usb_register_dev (interface, &tower_class);
	if (retval) {
		/* something prevented us from registering this driver */
		dev_err(idev, "Not able to get a minor for this device.\n");
		usb_set_intfdata (interface, NULL);
		goto error;
	}
	dev->minor = interface->minor;
	/* let the user know what node this device is now attached to */
	dev_info(&interface->dev, "LEGO USB Tower #%d now attached to major "
		 "%d minor %d\n", (dev->minor - LEGO_USB_TOWER_MINOR_BASE),
		 USB_MAJOR, dev->minor);
 	/* get the firmware version and log it */
 	result = usb_control_msg (udev,
 				  usb_rcvctrlpipe(udev, 0),
				  LEGO_USB_TOWER_REQUEST_GET_VERSION,
				  USB_TYPE_VENDOR | USB_DIR_IN | USB_RECIP_DEVICE,
				  0,
				  0,
				  &get_version_reply,
				  sizeof(get_version_reply),
				  1000);
	if (result < 0) {
		dev_err(idev, "LEGO USB Tower get version control request failed\n");
		retval = result;
		goto error;
	}
	dev_info(&interface->dev, "LEGO USB Tower firmware version is %d.%d "
		 "build %d\n", get_version_reply.major,
 		 get_version_reply.minor,
 		 le16_to_cpu(get_version_reply.build_no));
 
 
 exit:
 	return retval;

error:
	tower_delete(dev);
	return retval;
}
