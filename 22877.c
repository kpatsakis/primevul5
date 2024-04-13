void usb_remove_config(struct usb_composite_dev *cdev,
		      struct usb_configuration *config)
{
	unsigned long flags;

	spin_lock_irqsave(&cdev->lock, flags);

	if (cdev->config == config)
		reset_config(cdev);

	spin_unlock_irqrestore(&cdev->lock, flags);

	remove_config(cdev, config);
}