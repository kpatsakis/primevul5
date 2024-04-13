static struct mt_report_data *mt_allocate_report_data(struct mt_device *td,
						      struct hid_report *report)
{
	struct mt_report_data *rdata;
	struct hid_field *field;
	int r, n;

	rdata = devm_kzalloc(&td->hdev->dev, sizeof(*rdata), GFP_KERNEL);
	if (!rdata)
		return NULL;

	rdata->report = report;
	rdata->application = mt_find_application(td, report);

	if (!rdata->application) {
		devm_kfree(&td->hdev->dev, rdata);
		return NULL;
	}

	for (r = 0; r < report->maxfield; r++) {
		field = report->field[r];

		if (!(HID_MAIN_ITEM_VARIABLE & field->flags))
			continue;

		for (n = 0; n < field->report_count; n++) {
			if (field->usage[n].hid == HID_DG_CONTACTID)
				rdata->is_mt_collection = true;
		}
	}

	list_add_tail(&rdata->list, &td->reports);

	return rdata;
}