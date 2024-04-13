static struct mt_application *mt_find_application(struct mt_device *td,
						  struct hid_report *report)
{
	unsigned int application = report->application;
	struct mt_application *tmp, *mt_application = NULL;

	list_for_each_entry(tmp, &td->applications, list) {
		if (application == tmp->application) {
			if (!(td->mtclass.quirks & MT_QUIRK_SEPARATE_APP_REPORT) ||
			    tmp->report_id == report->id) {
				mt_application = tmp;
				break;
			}
		}
	}

	if (!mt_application)
		mt_application = mt_allocate_application(td, report);

	return mt_application;
}