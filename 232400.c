static void hidinput_cleanup_battery(struct hid_device *dev)
{
	const struct power_supply_desc *psy_desc;

	if (!dev->battery)
		return;

	psy_desc = dev->battery->desc;
	power_supply_unregister(dev->battery);
	kfree(psy_desc->name);
	kfree(psy_desc);
	dev->battery = NULL;
}