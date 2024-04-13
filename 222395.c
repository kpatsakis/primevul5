int input_flush_device(struct input_handle *handle, struct file *file)
{
	struct input_dev *dev = handle->dev;
	int retval;

	retval = mutex_lock_interruptible(&dev->mutex);
	if (retval)
		return retval;

	if (dev->flush)
		retval = dev->flush(dev, file);

	mutex_unlock(&dev->mutex);
	return retval;
}