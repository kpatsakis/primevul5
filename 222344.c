static ssize_t input_dev_show_properties(struct device *dev,
					 struct device_attribute *attr,
					 char *buf)
{
	struct input_dev *input_dev = to_input_dev(dev);
	int len = input_print_bitmap(buf, PAGE_SIZE, input_dev->propbit,
				     INPUT_PROP_MAX, true);
	return min_t(int, len, PAGE_SIZE);
}