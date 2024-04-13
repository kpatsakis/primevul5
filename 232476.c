static int mt_touch_input_configured(struct hid_device *hdev,
				     struct hid_input *hi,
				     struct mt_application *app)
{
	struct mt_device *td = hid_get_drvdata(hdev);
	struct mt_class *cls = &td->mtclass;
	struct input_dev *input = hi->input;
	int ret;

	if (!td->maxcontacts)
		td->maxcontacts = MT_DEFAULT_MAXCONTACT;

	mt_post_parse(td, app);
	if (td->serial_maybe)
		mt_post_parse_default_settings(td, app);

	if (cls->is_indirect)
		app->mt_flags |= INPUT_MT_POINTER;

	if (app->quirks & MT_QUIRK_NOT_SEEN_MEANS_UP)
		app->mt_flags |= INPUT_MT_DROP_UNUSED;

	/* check for clickpads */
	if ((app->mt_flags & INPUT_MT_POINTER) &&
	    (app->buttons_count == 1))
		td->is_buttonpad = true;

	if (td->is_buttonpad)
		__set_bit(INPUT_PROP_BUTTONPAD, input->propbit);

	app->pending_palm_slots = devm_kcalloc(&hi->input->dev,
					       BITS_TO_LONGS(td->maxcontacts),
					       sizeof(long),
					       GFP_KERNEL);
	if (!app->pending_palm_slots)
		return -ENOMEM;

	ret = input_mt_init_slots(input, td->maxcontacts, app->mt_flags);
	if (ret)
		return ret;

	app->mt_flags = 0;
	return 0;
}