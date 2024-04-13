static struct mt_report_data *mt_find_report_data(struct mt_device *td,
						  struct hid_report *report)
{
	struct mt_report_data *tmp, *rdata = NULL;

	list_for_each_entry(tmp, &td->reports, list) {
		if (report == tmp->report) {
			rdata = tmp;
			break;
		}
	}

	if (!rdata)
		rdata = mt_allocate_report_data(td, report);

	return rdata;
}