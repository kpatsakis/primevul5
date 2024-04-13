static struct hid_input *hidinput_match(struct hid_report *report)
{
	struct hid_device *hid = report->device;
	struct hid_input *hidinput;

	list_for_each_entry(hidinput, &hid->inputs, list) {
		if (hidinput->report &&
		    hidinput->report->id == report->id)
			return hidinput;
	}

	return NULL;
}