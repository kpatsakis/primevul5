unsigned int hidinput_count_leds(struct hid_device *hid)
{
	struct hid_report *report;
	struct hid_field *field;
	int i, j;
	unsigned int count = 0;

	list_for_each_entry(report,
			    &hid->report_enum[HID_OUTPUT_REPORT].report_list,
			    list) {
		for (i = 0; i < report->maxfield; i++) {
			field = report->field[i];
			for (j = 0; j < field->maxusage; j++)
				if (field->usage[j].type == EV_LED &&
				    field->value[j])
					count += 1;
		}
	}
	return count;
}