static int hidinput_setup_battery(struct hid_device *dev, unsigned report_type, struct hid_field *field)
{
	struct power_supply_desc *psy_desc;
	struct power_supply_config psy_cfg = { .drv_data = dev, };
	unsigned quirks;
	s32 min, max;
	int error;

	if (dev->battery)
		return 0;	/* already initialized? */

	quirks = find_battery_quirk(dev);

	hid_dbg(dev, "device %x:%x:%x %d quirks %d\n",
		dev->bus, dev->vendor, dev->product, dev->version, quirks);

	if (quirks & HID_BATTERY_QUIRK_IGNORE)
		return 0;

	psy_desc = kzalloc(sizeof(*psy_desc), GFP_KERNEL);
	if (!psy_desc)
		return -ENOMEM;

	psy_desc->name = kasprintf(GFP_KERNEL, "hid-%s-battery",
				   strlen(dev->uniq) ?
					dev->uniq : dev_name(&dev->dev));
	if (!psy_desc->name) {
		error = -ENOMEM;
		goto err_free_mem;
	}

	psy_desc->type = POWER_SUPPLY_TYPE_BATTERY;
	psy_desc->properties = hidinput_battery_props;
	psy_desc->num_properties = ARRAY_SIZE(hidinput_battery_props);
	psy_desc->use_for_apm = 0;
	psy_desc->get_property = hidinput_get_battery_property;

	min = field->logical_minimum;
	max = field->logical_maximum;

	if (quirks & HID_BATTERY_QUIRK_PERCENT) {
		min = 0;
		max = 100;
	}

	if (quirks & HID_BATTERY_QUIRK_FEATURE)
		report_type = HID_FEATURE_REPORT;

	dev->battery_min = min;
	dev->battery_max = max;
	dev->battery_report_type = report_type;
	dev->battery_report_id = field->report->id;

	/*
	 * Stylus is normally not connected to the device and thus we
	 * can't query the device and get meaningful battery strength.
	 * We have to wait for the device to report it on its own.
	 */
	dev->battery_avoid_query = report_type == HID_INPUT_REPORT &&
				   field->physical == HID_DG_STYLUS;

	dev->battery = power_supply_register(&dev->dev, psy_desc, &psy_cfg);
	if (IS_ERR(dev->battery)) {
		error = PTR_ERR(dev->battery);
		hid_warn(dev, "can't register power supply: %d\n", error);
		goto err_free_name;
	}

	power_supply_powers(dev->battery, &dev->dev);
	return 0;

err_free_name:
	kfree(psy_desc->name);
err_free_mem:
	kfree(psy_desc);
	dev->battery = NULL;
	return error;
}