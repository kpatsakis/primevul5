static void mt_fix_const_fields(struct hid_device *hdev, unsigned int usage)
{
	struct hid_report *report;
	int i;

	list_for_each_entry(report,
			    &hdev->report_enum[HID_INPUT_REPORT].report_list,
			    list) {

		if (!report->maxfield)
			continue;

		for (i = 0; i < report->maxfield; i++)
			if (report->field[i]->maxusage >= 1)
				mt_fix_const_field(report->field[i], usage);
	}
}