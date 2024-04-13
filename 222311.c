void input_alloc_absinfo(struct input_dev *dev)
{
	if (dev->absinfo)
		return;

	dev->absinfo = kcalloc(ABS_CNT, sizeof(*dev->absinfo), GFP_KERNEL);
	if (!dev->absinfo) {
		dev_err(dev->dev.parent ?: &dev->dev,
			"%s: unable to allocate memory\n", __func__);
		/*
		 * We will handle this allocation failure in
		 * input_register_device() when we refuse to register input
		 * device with ABS bits but without absinfo.
		 */
	}
}