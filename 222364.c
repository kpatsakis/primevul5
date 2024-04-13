static void input_pass_event(struct input_dev *dev,
			     unsigned int type, unsigned int code, int value)
{
	struct input_value vals[] = { { type, code, value } };

	input_pass_values(dev, vals, ARRAY_SIZE(vals));
}