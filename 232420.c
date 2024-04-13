static void mt_release_contacts(struct hid_device *hid)
{
	struct hid_input *hidinput;
	struct mt_application *application;
	struct mt_device *td = hid_get_drvdata(hid);

	list_for_each_entry(hidinput, &hid->inputs, list) {
		struct input_dev *input_dev = hidinput->input;
		struct input_mt *mt = input_dev->mt;
		int i;

		if (mt) {
			for (i = 0; i < mt->num_slots; i++) {
				input_mt_slot(input_dev, i);
				input_mt_report_slot_inactive(input_dev);
			}
			input_mt_sync_frame(input_dev);
			input_sync(input_dev);
		}
	}

	list_for_each_entry(application, &td->applications, list) {
		application->num_received = 0;
	}
}