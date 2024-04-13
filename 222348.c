struct input_dev *devm_input_allocate_device(struct device *dev)
{
	struct input_dev *input;
	struct input_devres *devres;

	devres = devres_alloc(devm_input_device_release,
			      sizeof(*devres), GFP_KERNEL);
	if (!devres)
		return NULL;

	input = input_allocate_device();
	if (!input) {
		devres_free(devres);
		return NULL;
	}

	input->dev.parent = dev;
	input->devres_managed = true;

	devres->input = input;
	devres_add(dev, devres);

	return input;
}