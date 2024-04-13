static void mt_report(struct hid_device *hid, struct hid_report *report)
{
	struct mt_device *td = hid_get_drvdata(hid);
	struct hid_field *field = report->field[0];
	struct mt_report_data *rdata;

	if (!(hid->claimed & HID_CLAIMED_INPUT))
		return;

	rdata = mt_find_report_data(td, report);
	if (rdata && rdata->is_mt_collection)
		return mt_touch_report(hid, rdata);

	if (field && field->hidinput && field->hidinput->input)
		input_sync(field->hidinput->input);
}