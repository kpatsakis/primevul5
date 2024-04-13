int input_open_device(struct input_handle *handle)
{
	struct input_dev *dev = handle->dev;
	int retval;

	retval = mutex_lock_interruptible(&dev->mutex);
	if (retval)
		return retval;

	if (dev->going_away) {
		retval = -ENODEV;
		goto out;
	}

	handle->open++;

	if (dev->users++) {
		/*
		 * Device is already opened, so we can exit immediately and
		 * report success.
		 */
		goto out;
	}

	if (dev->open) {
		retval = dev->open(dev);
		if (retval) {
			dev->users--;
			handle->open--;
			/*
			 * Make sure we are not delivering any more events
			 * through this handle
			 */
			synchronize_rcu();
			goto out;
		}
	}

	if (dev->poller)
		input_dev_poller_start(dev->poller);

 out:
	mutex_unlock(&dev->mutex);
	return retval;
}