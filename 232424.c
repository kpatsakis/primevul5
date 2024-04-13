static int mt_input_configured(struct hid_device *hdev, struct hid_input *hi)
{
	struct mt_device *td = hid_get_drvdata(hdev);
	char *name;
	const char *suffix = NULL;
	struct mt_report_data *rdata;
	struct mt_application *mt_application = NULL;
	struct hid_report *report;
	int ret;

	list_for_each_entry(report, &hi->reports, hidinput_list) {
		rdata = mt_find_report_data(td, report);
		if (!rdata) {
			hid_err(hdev, "failed to allocate data for report\n");
			return -ENOMEM;
		}

		mt_application = rdata->application;

		if (rdata->is_mt_collection) {
			ret = mt_touch_input_configured(hdev, hi,
							mt_application);
			if (ret)
				return ret;
		}
	}

	switch (hi->application) {
	case HID_GD_KEYBOARD:
	case HID_GD_KEYPAD:
	case HID_GD_MOUSE:
	case HID_DG_TOUCHPAD:
	case HID_GD_SYSTEM_CONTROL:
	case HID_CP_CONSUMER_CONTROL:
	case HID_GD_WIRELESS_RADIO_CTLS:
	case HID_GD_SYSTEM_MULTIAXIS:
		/* already handled by hid core */
		break;
	case HID_DG_TOUCHSCREEN:
		/* we do not set suffix = "Touchscreen" */
		hi->input->name = hdev->name;
		break;
	case HID_DG_STYLUS:
		/* force BTN_STYLUS to allow tablet matching in udev */
		__set_bit(BTN_STYLUS, hi->input->keybit);
		break;
	case HID_VD_ASUS_CUSTOM_MEDIA_KEYS:
		suffix = "Custom Media Keys";
		break;
	case HID_DG_PEN:
		suffix = "Stylus";
		break;
	default:
		suffix = "UNKNOWN";
		break;
	}

	if (suffix) {
		name = devm_kzalloc(&hi->input->dev,
				    strlen(hdev->name) + strlen(suffix) + 2,
				    GFP_KERNEL);
		if (name) {
			sprintf(name, "%s %s", hdev->name, suffix);
			hi->input->name = name;
		}
	}

	return 0;
}