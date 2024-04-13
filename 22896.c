static void __composite_disconnect(struct usb_gadget *gadget)
{
	struct usb_composite_dev	*cdev = get_gadget_data(gadget);
	unsigned long			flags;

	/* REVISIT:  should we have config and device level
	 * disconnect callbacks?
	 */
	spin_lock_irqsave(&cdev->lock, flags);
	cdev->suspended = 0;
	if (cdev->config)
		reset_config(cdev);
	if (cdev->driver->disconnect)
		cdev->driver->disconnect(cdev);
	spin_unlock_irqrestore(&cdev->lock, flags);
}