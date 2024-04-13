int usb_function_deactivate(struct usb_function *function)
{
	struct usb_composite_dev	*cdev = function->config->cdev;
	unsigned long			flags;
	int				status = 0;

	spin_lock_irqsave(&cdev->lock, flags);

	if (cdev->deactivations == 0) {
		spin_unlock_irqrestore(&cdev->lock, flags);
		status = usb_gadget_deactivate(cdev->gadget);
		spin_lock_irqsave(&cdev->lock, flags);
	}
	if (status == 0)
		cdev->deactivations++;

	spin_unlock_irqrestore(&cdev->lock, flags);
	return status;
}