static int input_dev_poweroff(struct device *dev)
{
	struct input_dev *input_dev = to_input_dev(dev);

	spin_lock_irq(&input_dev->event_lock);

	/* Turn off LEDs and sounds, if any are active. */
	input_dev_toggle(input_dev, false);

	spin_unlock_irq(&input_dev->event_lock);

	return 0;
}