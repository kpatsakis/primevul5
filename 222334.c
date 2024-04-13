static void input_dev_release(struct device *device)
{
	struct input_dev *dev = to_input_dev(device);

	input_ff_destroy(dev);
	input_mt_destroy_slots(dev);
	kfree(dev->poller);
	kfree(dev->absinfo);
	kfree(dev->vals);
	kfree(dev);

	module_put(THIS_MODULE);
}