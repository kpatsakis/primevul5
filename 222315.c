void input_reset_device(struct input_dev *dev)
{
	unsigned long flags;

	mutex_lock(&dev->mutex);
	spin_lock_irqsave(&dev->event_lock, flags);

	input_dev_toggle(dev, true);
	input_dev_release_keys(dev);

	spin_unlock_irqrestore(&dev->event_lock, flags);
	mutex_unlock(&dev->mutex);
}