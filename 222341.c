void input_release_device(struct input_handle *handle)
{
	struct input_dev *dev = handle->dev;

	mutex_lock(&dev->mutex);
	__input_release_device(handle);
	mutex_unlock(&dev->mutex);
}