static int input_dev_uevent(struct device *device, struct kobj_uevent_env *env)
{
	struct input_dev *dev = to_input_dev(device);

	INPUT_ADD_HOTPLUG_VAR("PRODUCT=%x/%x/%x/%x",
				dev->id.bustype, dev->id.vendor,
				dev->id.product, dev->id.version);
	if (dev->name)
		INPUT_ADD_HOTPLUG_VAR("NAME=\"%s\"", dev->name);
	if (dev->phys)
		INPUT_ADD_HOTPLUG_VAR("PHYS=\"%s\"", dev->phys);
	if (dev->uniq)
		INPUT_ADD_HOTPLUG_VAR("UNIQ=\"%s\"", dev->uniq);

	INPUT_ADD_HOTPLUG_BM_VAR("PROP=", dev->propbit, INPUT_PROP_MAX);

	INPUT_ADD_HOTPLUG_BM_VAR("EV=", dev->evbit, EV_MAX);
	if (test_bit(EV_KEY, dev->evbit))
		INPUT_ADD_HOTPLUG_BM_VAR("KEY=", dev->keybit, KEY_MAX);
	if (test_bit(EV_REL, dev->evbit))
		INPUT_ADD_HOTPLUG_BM_VAR("REL=", dev->relbit, REL_MAX);
	if (test_bit(EV_ABS, dev->evbit))
		INPUT_ADD_HOTPLUG_BM_VAR("ABS=", dev->absbit, ABS_MAX);
	if (test_bit(EV_MSC, dev->evbit))
		INPUT_ADD_HOTPLUG_BM_VAR("MSC=", dev->mscbit, MSC_MAX);
	if (test_bit(EV_LED, dev->evbit))
		INPUT_ADD_HOTPLUG_BM_VAR("LED=", dev->ledbit, LED_MAX);
	if (test_bit(EV_SND, dev->evbit))
		INPUT_ADD_HOTPLUG_BM_VAR("SND=", dev->sndbit, SND_MAX);
	if (test_bit(EV_FF, dev->evbit))
		INPUT_ADD_HOTPLUG_BM_VAR("FF=", dev->ffbit, FF_MAX);
	if (test_bit(EV_SW, dev->evbit))
		INPUT_ADD_HOTPLUG_BM_VAR("SW=", dev->swbit, SW_MAX);

	INPUT_ADD_HOTPLUG_MODALIAS_VAR(dev);

	return 0;
}