static int mt_compute_slot(struct mt_device *td, struct mt_application *app,
			   struct mt_usages *slot,
			   struct input_dev *input)
{
	__s32 quirks = app->quirks;

	if (quirks & MT_QUIRK_SLOT_IS_CONTACTID)
		return *slot->contactid;

	if (quirks & MT_QUIRK_CYPRESS)
		return cypress_compute_slot(app, slot);

	if (quirks & MT_QUIRK_SLOT_IS_CONTACTNUMBER)
		return app->num_received;

	if (quirks & MT_QUIRK_SLOT_IS_CONTACTID_MINUS_ONE)
		return *slot->contactid - 1;

	return input_mt_get_slot_by_key(input, *slot->contactid);
}