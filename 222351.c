static void input_disconnect_device(struct input_dev *dev)
{
	struct input_handle *handle;

	/*
	 * Mark device as going away. Note that we take dev->mutex here
	 * not to protect access to dev->going_away but rather to ensure
	 * that there are no threads in the middle of input_open_device()
	 */
	mutex_lock(&dev->mutex);
	dev->going_away = true;
	mutex_unlock(&dev->mutex);

	spin_lock_irq(&dev->event_lock);

	/*
	 * Simulate keyup events for all pressed keys so that handlers
	 * are not left with "stuck" keys. The driver may continue
	 * generate events even after we done here but they will not
	 * reach any handlers.
	 */
	input_dev_release_keys(dev);

	list_for_each_entry(handle, &dev->h_list, d_node)
		handle->open = 0;

	spin_unlock_irq(&dev->event_lock);
}