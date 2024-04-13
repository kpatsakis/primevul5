static void hidinput_update_battery(struct hid_device *dev, int value)
{
	int capacity;

	if (!dev->battery)
		return;

	if (value == 0 || value < dev->battery_min || value > dev->battery_max)
		return;

	capacity = hidinput_scale_battery_capacity(dev, value);

	if (dev->battery_status != HID_BATTERY_REPORTED ||
	    capacity != dev->battery_capacity) {
		dev->battery_capacity = capacity;
		dev->battery_status = HID_BATTERY_REPORTED;
		power_supply_changed(dev->battery);
	}
}