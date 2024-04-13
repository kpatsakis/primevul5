static int input_dev_resume(struct device *dev)
{
	struct input_dev *input_dev = to_input_dev(dev);

	spin_lock_irq(&input_dev->event_lock);

	/* Restore state of LEDs and sounds, if any were active. */
	input_dev_toggle(input_dev, true);

	spin_unlock_irq(&input_dev->event_lock);

	return 0;
}