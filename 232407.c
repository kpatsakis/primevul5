static int hidinput_scale_battery_capacity(struct hid_device *dev,
					   int value)
{
	if (dev->battery_min < dev->battery_max &&
	    value >= dev->battery_min && value <= dev->battery_max)
		value = ((value - dev->battery_min) * 100) /
			(dev->battery_max - dev->battery_min);

	return value;
}