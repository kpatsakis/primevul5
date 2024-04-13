static void printer_reset_interface(struct printer_dev *dev)
{
	unsigned long	flags;

	if (dev->interface < 0)
		return;

	DBG(dev, "%s\n", __func__);

	if (dev->in_ep->desc)
		usb_ep_disable(dev->in_ep);

	if (dev->out_ep->desc)
		usb_ep_disable(dev->out_ep);

	spin_lock_irqsave(&dev->lock, flags);
	dev->in_ep->desc = NULL;
	dev->out_ep->desc = NULL;
	dev->interface = -1;
	spin_unlock_irqrestore(&dev->lock, flags);
}