static ssize_t mt_set_quirks(struct device *dev,
			  struct device_attribute *attr,
			  const char *buf, size_t count)
{
	struct hid_device *hdev = to_hid_device(dev);
	struct mt_device *td = hid_get_drvdata(hdev);
	struct mt_application *application;

	unsigned long val;

	if (kstrtoul(buf, 0, &val))
		return -EINVAL;

	td->mtclass.quirks = val;

	list_for_each_entry(application, &td->applications, list) {
		application->quirks = val;
		if (!application->have_contact_count)
			application->quirks &= ~MT_QUIRK_CONTACT_CNT_ACCURATE;
	}

	return count;
}