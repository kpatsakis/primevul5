int usb_function_activate(struct usb_function *function)
{
	struct usb_composite_dev	*cdev = function->config->cdev;
	unsigned long			flags;
	int				status = 0;

	spin_lock_irqsave(&cdev->lock, flags);

	if (WARN_ON(cdev->deactivations == 0))
		status = -EINVAL;
	else {
		cdev->deactivations--;
		if (cdev->deactivations == 0) {
			spin_unlock_irqrestore(&cdev->lock, flags);
			status = usb_gadget_activate(cdev->gadget);
			spin_lock_irqsave(&cdev->lock, flags);
		}
	}

	spin_unlock_irqrestore(&cdev->lock, flags);
	return status;
}