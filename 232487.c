static struct hid_input *hidinput_allocate(struct hid_device *hid,
					   unsigned int application)
{
	struct hid_input *hidinput = kzalloc(sizeof(*hidinput), GFP_KERNEL);
	struct input_dev *input_dev = input_allocate_device();
	const char *suffix = NULL;
	size_t suffix_len, name_len;

	if (!hidinput || !input_dev)
		goto fail;

	if ((hid->quirks & HID_QUIRK_INPUT_PER_APP) &&
	    hid->maxapplication > 1) {
		switch (application) {
		case HID_GD_KEYBOARD:
			suffix = "Keyboard";
			break;
		case HID_GD_KEYPAD:
			suffix = "Keypad";
			break;
		case HID_GD_MOUSE:
			suffix = "Mouse";
			break;
		case HID_DG_STYLUS:
			suffix = "Pen";
			break;
		case HID_DG_TOUCHSCREEN:
			suffix = "Touchscreen";
			break;
		case HID_DG_TOUCHPAD:
			suffix = "Touchpad";
			break;
		case HID_GD_SYSTEM_CONTROL:
			suffix = "System Control";
			break;
		case HID_CP_CONSUMER_CONTROL:
			suffix = "Consumer Control";
			break;
		case HID_GD_WIRELESS_RADIO_CTLS:
			suffix = "Wireless Radio Control";
			break;
		case HID_GD_SYSTEM_MULTIAXIS:
			suffix = "System Multi Axis";
			break;
		default:
			break;
		}
	}

	if (suffix) {
		name_len = strlen(hid->name);
		suffix_len = strlen(suffix);
		if ((name_len < suffix_len) ||
		    strcmp(hid->name + name_len - suffix_len, suffix)) {
			hidinput->name = kasprintf(GFP_KERNEL, "%s %s",
						   hid->name, suffix);
			if (!hidinput->name)
				goto fail;
		}
	}

	input_set_drvdata(input_dev, hid);
	input_dev->event = hidinput_input_event;
	input_dev->open = hidinput_open;
	input_dev->close = hidinput_close;
	input_dev->setkeycode = hidinput_setkeycode;
	input_dev->getkeycode = hidinput_getkeycode;

	input_dev->name = hidinput->name ? hidinput->name : hid->name;
	input_dev->phys = hid->phys;
	input_dev->uniq = hid->uniq;
	input_dev->id.bustype = hid->bus;
	input_dev->id.vendor  = hid->vendor;
	input_dev->id.product = hid->product;
	input_dev->id.version = hid->version;
	input_dev->dev.parent = &hid->dev;

	hidinput->input = input_dev;
	hidinput->application = application;
	list_add_tail(&hidinput->list, &hid->inputs);

	INIT_LIST_HEAD(&hidinput->reports);

	return hidinput;

fail:
	kfree(hidinput);
	input_free_device(input_dev);
	hid_err(hid, "Out of memory during hid input probe\n");
	return NULL;
}