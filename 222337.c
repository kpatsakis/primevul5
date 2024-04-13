int input_get_keycode(struct input_dev *dev, struct input_keymap_entry *ke)
{
	unsigned long flags;
	int retval;

	spin_lock_irqsave(&dev->event_lock, flags);
	retval = dev->getkeycode(dev, ke);
	spin_unlock_irqrestore(&dev->event_lock, flags);

	return retval;
}