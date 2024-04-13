static void mt_set_modes(struct hid_device *hdev, enum latency_mode latency,
			 bool surface_switch, bool button_switch)
{
	struct hid_report_enum *rep_enum;
	struct hid_report *rep;
	struct hid_usage *usage;
	int i, j;
	bool update_report;
	bool inputmode_found = false;

	rep_enum = &hdev->report_enum[HID_FEATURE_REPORT];
	list_for_each_entry(rep, &rep_enum->report_list, list) {
		update_report = false;

		for (i = 0; i < rep->maxfield; i++) {
			/* Ignore if report count is out of bounds. */
			if (rep->field[i]->report_count < 1)
				continue;

			for (j = 0; j < rep->field[i]->maxusage; j++) {
				usage = &rep->field[i]->usage[j];

				if (mt_need_to_apply_feature(hdev,
							     rep->field[i],
							     usage,
							     latency,
							     surface_switch,
							     button_switch,
							     &inputmode_found))
					update_report = true;
			}
		}

		if (update_report)
			hid_hw_request(hdev, rep, HID_REQ_SET_REPORT);
	}
}