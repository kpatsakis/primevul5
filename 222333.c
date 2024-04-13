static char *input_devnode(struct device *dev, umode_t *mode)
{
	return kasprintf(GFP_KERNEL, "input/%s", dev_name(dev));
}