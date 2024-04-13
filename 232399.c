void hidinput_report_event(struct hid_device *hid, struct hid_report *report)
{
	struct hid_input *hidinput;

	if (hid->quirks & HID_QUIRK_NO_INPUT_SYNC)
		return;

	list_for_each_entry(hidinput, &hid->inputs, list)
		input_sync(hidinput->input);
}