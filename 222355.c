void input_unregister_handle(struct input_handle *handle)
{
	struct input_dev *dev = handle->dev;

	list_del_rcu(&handle->h_node);

	/*
	 * Take dev->mutex to prevent race with input_release_device().
	 */
	mutex_lock(&dev->mutex);
	list_del_rcu(&handle->d_node);
	mutex_unlock(&dev->mutex);

	synchronize_rcu();
}