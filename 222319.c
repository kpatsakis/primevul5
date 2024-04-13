int input_grab_device(struct input_handle *handle)
{
	struct input_dev *dev = handle->dev;
	int retval;

	retval = mutex_lock_interruptible(&dev->mutex);
	if (retval)
		return retval;

	if (dev->grab) {
		retval = -EBUSY;
		goto out;
	}

	rcu_assign_pointer(dev->grab, handle);

 out:
	mutex_unlock(&dev->mutex);
	return retval;
}