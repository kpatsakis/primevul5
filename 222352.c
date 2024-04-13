static int devm_input_device_match(struct device *dev, void *res, void *data)
{
	struct input_devres *devres = res;

	return devres->input == data;
}