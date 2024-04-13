static int hidinput_getkeycode(struct input_dev *dev,
			       struct input_keymap_entry *ke)
{
	struct hid_device *hid = input_get_drvdata(dev);
	struct hid_usage *usage;
	unsigned int scancode, index;

	usage = hidinput_locate_usage(hid, ke, &index);
	if (usage) {
		ke->keycode = usage->type == EV_KEY ?
				usage->code : KEY_RESERVED;
		ke->index = index;
		scancode = usage->hid & (HID_USAGE_PAGE | HID_USAGE);
		ke->len = sizeof(scancode);
		memcpy(ke->scancode, &scancode, sizeof(scancode));
		return 0;
	}

	return -EINVAL;
}