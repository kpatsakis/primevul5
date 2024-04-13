void hidinput_disconnect(struct hid_device *hid)
{
	struct hid_input *hidinput, *next;

	hidinput_cleanup_battery(hid);

	list_for_each_entry_safe(hidinput, next, &hid->inputs, list) {
		list_del(&hidinput->list);
		if (hidinput->registered)
			input_unregister_device(hidinput->input);
		else
			input_free_device(hidinput->input);
		kfree(hidinput->name);
		kfree(hidinput);
	}

	/* led_work is spawned by input_dev callbacks, but doesn't access the
	 * parent input_dev at all. Once all input devices are removed, we
	 * know that led_work will never get restarted, so we can cancel it
	 * synchronously and are safe. */
	cancel_work_sync(&hid->led_work);
}