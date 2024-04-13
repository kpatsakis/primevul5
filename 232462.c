static bool __hidinput_change_resolution_multipliers(struct hid_device *hid,
		struct hid_report *report, bool use_logical_max)
{
	struct hid_usage *usage;
	bool update_needed = false;
	bool get_report_completed = false;
	int i, j;

	if (report->maxfield == 0)
		return false;

	for (i = 0; i < report->maxfield; i++) {
		__s32 value = use_logical_max ?
			      report->field[i]->logical_maximum :
			      report->field[i]->logical_minimum;

		/* There is no good reason for a Resolution
		 * Multiplier to have a count other than 1.
		 * Ignore that case.
		 */
		if (report->field[i]->report_count != 1)
			continue;

		for (j = 0; j < report->field[i]->maxusage; j++) {
			usage = &report->field[i]->usage[j];

			if (usage->hid != HID_GD_RESOLUTION_MULTIPLIER)
				continue;

			/*
			 * If we have more than one feature within this
			 * report we need to fill in the bits from the
			 * others before we can overwrite the ones for the
			 * Resolution Multiplier.
			 *
			 * But if we're not allowed to read from the device,
			 * we just bail. Such a device should not exist
			 * anyway.
			 */
			if (!get_report_completed && report->maxfield > 1) {
				if (hid->quirks & HID_QUIRK_NO_INIT_REPORTS)
					return update_needed;

				hid_hw_request(hid, report, HID_REQ_GET_REPORT);
				hid_hw_wait(hid);
				get_report_completed = true;
			}

			report->field[i]->value[j] = value;
			update_needed = true;
		}
	}

	return update_needed;
}