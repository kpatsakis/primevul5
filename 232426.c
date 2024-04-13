static unsigned find_battery_quirk(struct hid_device *hdev)
{
	unsigned quirks = 0;
	const struct hid_device_id *match;

	match = hid_match_id(hdev, hid_battery_quirks);
	if (match != NULL)
		quirks = match->driver_data;

	return quirks;
}