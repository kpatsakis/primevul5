static int input_dev_freeze(struct device *dev)
{
	struct input_dev *input_dev = to_input_dev(dev);

	spin_lock_irq(&input_dev->event_lock);

	/*
	 * Keys that are pressed now are unlikely to be
	 * still pressed when we resume.
	 */
	input_dev_release_keys(input_dev);

	spin_unlock_irq(&input_dev->event_lock);

	return 0;
}