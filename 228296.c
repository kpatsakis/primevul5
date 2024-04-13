static int toggle_sync_simple(struct usbtest_dev *dev)
{
	int			ep;
	int			retval = 0;
	struct urb		*urb;
	struct usb_device	*udev = testdev_to_usbdev(dev);
	unsigned		maxp = get_maxpacket(udev, dev->out_pipe);

	/*
	 * Create a URB that causes a transfer of uneven amount of data packets
	 * This way the clear toggle has an impact on the data toggle sequence.
	 * Use 2 maxpacket length packets and one zero packet.
	 */
	urb = simple_alloc_urb(udev, 0,  2 * maxp, 0);
	if (urb == NULL)
		return -ENOMEM;

	urb->transfer_flags |= URB_ZERO_PACKET;

	ep = usb_pipeendpoint(dev->out_pipe);
	urb->pipe = dev->out_pipe;
	retval = test_toggle_sync(dev, ep, urb);

	simple_free_urb(urb);
	return retval;
}