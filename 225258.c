static ssize_t evdev_write(struct file *file, const char __user *buffer,
			   size_t count, loff_t *ppos)
{
	struct evdev_client *client = file->private_data;
	struct evdev *evdev = client->evdev;
	struct input_event event;
	int retval = 0;

	if (count != 0 && count < input_event_size())
		return -EINVAL;

	retval = mutex_lock_interruptible(&evdev->mutex);
	if (retval)
		return retval;

	if (!evdev->exist) {
		retval = -ENODEV;
		goto out;
	}

	while (retval + input_event_size() <= count) {

		if (input_event_from_user(buffer + retval, &event)) {
			retval = -EFAULT;
			goto out;
		}
		retval += input_event_size();

		input_inject_event(&evdev->handle,
				   event.type, event.code, event.value);
	}

 out:
	mutex_unlock(&evdev->mutex);
	return retval;
}