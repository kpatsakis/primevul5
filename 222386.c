static void devm_input_device_release(struct device *dev, void *res)
{
	struct input_devres *devres = res;
	struct input_dev *input = devres->input;

	dev_dbg(dev, "%s: dropping reference to %s\n",
		__func__, dev_name(&input->dev));
	input_put_device(input);
}