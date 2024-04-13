static void mt_release_pending_palms(struct mt_device *td,
				     struct mt_application *app,
				     struct input_dev *input)
{
	int slotnum;
	bool need_sync = false;

	for_each_set_bit(slotnum, app->pending_palm_slots, td->maxcontacts) {
		clear_bit(slotnum, app->pending_palm_slots);

		input_mt_slot(input, slotnum);
		input_mt_report_slot_inactive(input);

		need_sync = true;
	}

	if (need_sync) {
		input_mt_sync_frame(input);
		input_sync(input);
	}
}