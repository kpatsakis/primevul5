static void hidinput_cleanup_hidinput(struct hid_device *hid,
		struct hid_input *hidinput)
{
	struct hid_report *report;
	int i, k;

	list_del(&hidinput->list);
	input_free_device(hidinput->input);
	kfree(hidinput->name);

	for (k = HID_INPUT_REPORT; k <= HID_OUTPUT_REPORT; k++) {
		if (k == HID_OUTPUT_REPORT &&
			hid->quirks & HID_QUIRK_SKIP_OUTPUT_REPORTS)
			continue;

		list_for_each_entry(report, &hid->report_enum[k].report_list,
				    list) {

			for (i = 0; i < report->maxfield; i++)
				if (report->field[i]->hidinput == hidinput)
					report->field[i]->hidinput = NULL;
		}
	}

	kfree(hidinput);
}