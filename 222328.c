void input_free_device(struct input_dev *dev)
{
	if (dev) {
		if (dev->devres_managed)
			WARN_ON(devres_destroy(dev->dev.parent,
						devm_input_device_release,
						devm_input_device_match,
						dev));
		input_put_device(dev);
	}
}