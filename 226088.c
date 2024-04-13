static int set_interface(struct printer_dev *dev, unsigned number)
{
	int			result = 0;

	/* Free the current interface */
	printer_reset_interface(dev);

	result = set_printer_interface(dev);
	if (result)
		printer_reset_interface(dev);
	else
		dev->interface = number;

	if (!result)
		INFO(dev, "Using interface %x\n", number);

	return result;
}