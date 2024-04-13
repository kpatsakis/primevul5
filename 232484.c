static void mt_touch_report(struct hid_device *hid,
			    struct mt_report_data *rdata)
{
	struct mt_device *td = hid_get_drvdata(hid);
	struct hid_report *report = rdata->report;
	struct mt_application *app = rdata->application;
	struct hid_field *field;
	struct input_dev *input;
	struct mt_usages *slot;
	bool first_packet;
	unsigned count;
	int r, n;
	int scantime = 0;
	int contact_count = -1;

	/* sticky fingers release in progress, abort */
	if (test_and_set_bit(MT_IO_FLAGS_RUNNING, &td->mt_io_flags))
		return;

	scantime = *app->scantime;
	app->timestamp = mt_compute_timestamp(app, scantime);
	if (app->raw_cc != DEFAULT_ZERO)
		contact_count = *app->raw_cc;

	/*
	 * Includes multi-packet support where subsequent
	 * packets are sent with zero contactcount.
	 */
	if (contact_count >= 0) {
		/*
		 * For Win8 PTPs the first packet (td->num_received == 0) may
		 * have a contactcount of 0 if there only is a button event.
		 * We double check that this is not a continuation packet
		 * of a possible multi-packet frame be checking that the
		 * timestamp has changed.
		 */
		if ((app->quirks & MT_QUIRK_WIN8_PTP_BUTTONS) &&
		    app->num_received == 0 &&
		    app->prev_scantime != scantime)
			app->num_expected = contact_count;
		/* A non 0 contact count always indicates a first packet */
		else if (contact_count)
			app->num_expected = contact_count;
	}
	app->prev_scantime = scantime;

	first_packet = app->num_received == 0;

	input = report->field[0]->hidinput->input;

	list_for_each_entry(slot, &app->mt_usages, list) {
		if (!mt_process_slot(td, input, app, slot))
			app->num_received++;
	}

	for (r = 0; r < report->maxfield; r++) {
		field = report->field[r];
		count = field->report_count;

		if (!(HID_MAIN_ITEM_VARIABLE & field->flags))
			continue;

		for (n = 0; n < count; n++)
			mt_process_mt_event(hid, app, field,
					    &field->usage[n], field->value[n],
					    first_packet);
	}

	if (app->num_received >= app->num_expected)
		mt_sync_frame(td, app, input);

	/*
	 * Windows 8 specs says 2 things:
	 * - once a contact has been reported, it has to be reported in each
	 *   subsequent report
	 * - the report rate when fingers are present has to be at least
	 *   the refresh rate of the screen, 60 or 120 Hz
	 *
	 * I interprete this that the specification forces a report rate of
	 * at least 60 Hz for a touchscreen to be certified.
	 * Which means that if we do not get a report whithin 16 ms, either
	 * something wrong happens, either the touchscreen forgets to send
	 * a release. Taking a reasonable margin allows to remove issues
	 * with USB communication or the load of the machine.
	 *
	 * Given that Win 8 devices are forced to send a release, this will
	 * only affect laggish machines and the ones that have a firmware
	 * defect.
	 */
	if (app->quirks & MT_QUIRK_STICKY_FINGERS) {
		if (test_bit(MT_IO_FLAGS_PENDING_SLOTS, &td->mt_io_flags))
			mod_timer(&td->release_timer,
				  jiffies + msecs_to_jiffies(100));
		else
			del_timer(&td->release_timer);
	}

	clear_bit(MT_IO_FLAGS_RUNNING, &td->mt_io_flags);
}