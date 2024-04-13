static int mt_event(struct hid_device *hid, struct hid_field *field,
				struct hid_usage *usage, __s32 value)
{
	struct mt_device *td = hid_get_drvdata(hid);
	struct mt_report_data *rdata;

	rdata = mt_find_report_data(td, field->report);
	if (rdata && rdata->is_mt_collection)
		return mt_touch_event(hid, field, usage, value);

	return 0;
}