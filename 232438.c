static struct mt_application *mt_allocate_application(struct mt_device *td,
						      struct hid_report *report)
{
	unsigned int application = report->application;
	struct mt_application *mt_application;

	mt_application = devm_kzalloc(&td->hdev->dev, sizeof(*mt_application),
				      GFP_KERNEL);
	if (!mt_application)
		return NULL;

	mt_application->application = application;
	INIT_LIST_HEAD(&mt_application->mt_usages);

	if (application == HID_DG_TOUCHSCREEN)
		mt_application->mt_flags |= INPUT_MT_DIRECT;

	/*
	 * Model touchscreens providing buttons as touchpads.
	 */
	if (application == HID_DG_TOUCHPAD) {
		mt_application->mt_flags |= INPUT_MT_POINTER;
		td->inputmode_value = MT_INPUTMODE_TOUCHPAD;
	}

	mt_application->scantime = DEFAULT_ZERO;
	mt_application->raw_cc = DEFAULT_ZERO;
	mt_application->quirks = td->mtclass.quirks;
	mt_application->report_id = report->id;

	list_add_tail(&mt_application->list, &td->applications);

	return mt_application;
}