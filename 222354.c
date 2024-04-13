bool input_match_device_id(const struct input_dev *dev,
			   const struct input_device_id *id)
{
	if (id->flags & INPUT_DEVICE_ID_MATCH_BUS)
		if (id->bustype != dev->id.bustype)
			return false;

	if (id->flags & INPUT_DEVICE_ID_MATCH_VENDOR)
		if (id->vendor != dev->id.vendor)
			return false;

	if (id->flags & INPUT_DEVICE_ID_MATCH_PRODUCT)
		if (id->product != dev->id.product)
			return false;

	if (id->flags & INPUT_DEVICE_ID_MATCH_VERSION)
		if (id->version != dev->id.version)
			return false;

	if (!bitmap_subset(id->evbit, dev->evbit, EV_MAX) ||
	    !bitmap_subset(id->keybit, dev->keybit, KEY_MAX) ||
	    !bitmap_subset(id->relbit, dev->relbit, REL_MAX) ||
	    !bitmap_subset(id->absbit, dev->absbit, ABS_MAX) ||
	    !bitmap_subset(id->mscbit, dev->mscbit, MSC_MAX) ||
	    !bitmap_subset(id->ledbit, dev->ledbit, LED_MAX) ||
	    !bitmap_subset(id->sndbit, dev->sndbit, SND_MAX) ||
	    !bitmap_subset(id->ffbit, dev->ffbit, FF_MAX) ||
	    !bitmap_subset(id->swbit, dev->swbit, SW_MAX) ||
	    !bitmap_subset(id->propbit, dev->propbit, INPUT_PROP_MAX)) {
		return false;
	}

	return true;
}