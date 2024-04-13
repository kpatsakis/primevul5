static int mt_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
	int ret, i;
	struct mt_device *td;
	const struct mt_class *mtclass = mt_classes; /* MT_CLS_DEFAULT */

	for (i = 0; mt_classes[i].name ; i++) {
		if (id->driver_data == mt_classes[i].name) {
			mtclass = &(mt_classes[i]);
			break;
		}
	}

	td = devm_kzalloc(&hdev->dev, sizeof(struct mt_device), GFP_KERNEL);
	if (!td) {
		dev_err(&hdev->dev, "cannot allocate multitouch data\n");
		return -ENOMEM;
	}
	td->hdev = hdev;
	td->mtclass = *mtclass;
	td->inputmode_value = MT_INPUTMODE_TOUCHSCREEN;
	hid_set_drvdata(hdev, td);

	INIT_LIST_HEAD(&td->applications);
	INIT_LIST_HEAD(&td->reports);

	if (id->vendor == HID_ANY_ID && id->product == HID_ANY_ID)
		td->serial_maybe = true;

	/* This allows the driver to correctly support devices
	 * that emit events over several HID messages.
	 */
	hdev->quirks |= HID_QUIRK_NO_INPUT_SYNC;

	/*
	 * This allows the driver to handle different input sensors
	 * that emits events through different applications on the same HID
	 * device.
	 */
	hdev->quirks |= HID_QUIRK_INPUT_PER_APP;

	if (id->group != HID_GROUP_MULTITOUCH_WIN_8)
		hdev->quirks |= HID_QUIRK_MULTI_INPUT;

	if (mtclass->quirks & MT_QUIRK_FORCE_MULTI_INPUT) {
		hdev->quirks &= ~HID_QUIRK_INPUT_PER_APP;
		hdev->quirks |= HID_QUIRK_MULTI_INPUT;
	}

	timer_setup(&td->release_timer, mt_expired_timeout, 0);

	ret = hid_parse(hdev);
	if (ret != 0)
		return ret;

	if (mtclass->quirks & MT_QUIRK_FIX_CONST_CONTACT_ID)
		mt_fix_const_fields(hdev, HID_DG_CONTACTID);

	ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
	if (ret)
		return ret;

	ret = sysfs_create_group(&hdev->dev.kobj, &mt_attribute_group);
	if (ret)
		dev_warn(&hdev->dev, "Cannot allocate sysfs group for %s\n",
				hdev->name);

	mt_set_modes(hdev, HID_LATENCY_NORMAL, true, true);

	return 0;
}