int hidinput_connect(struct hid_device *hid, unsigned int force)
{
	struct hid_driver *drv = hid->driver;
	struct hid_report *report;
	struct hid_input *next, *hidinput = NULL;
	unsigned int application;
	int i, k;

	INIT_LIST_HEAD(&hid->inputs);
	INIT_WORK(&hid->led_work, hidinput_led_worker);

	hid->status &= ~HID_STAT_DUP_DETECTED;

	if (!force) {
		for (i = 0; i < hid->maxcollection; i++) {
			struct hid_collection *col = &hid->collection[i];
			if (col->type == HID_COLLECTION_APPLICATION ||
					col->type == HID_COLLECTION_PHYSICAL)
				if (IS_INPUT_APPLICATION(col->usage))
					break;
		}

		if (i == hid->maxcollection)
			return -1;
	}

	report_features(hid);

	for (k = HID_INPUT_REPORT; k <= HID_OUTPUT_REPORT; k++) {
		if (k == HID_OUTPUT_REPORT &&
			hid->quirks & HID_QUIRK_SKIP_OUTPUT_REPORTS)
			continue;

		list_for_each_entry(report, &hid->report_enum[k].report_list, list) {

			if (!report->maxfield)
				continue;

			application = report->application;

			/*
			 * Find the previous hidinput report attached
			 * to this report id.
			 */
			if (hid->quirks & HID_QUIRK_MULTI_INPUT)
				hidinput = hidinput_match(report);
			else if (hid->maxapplication > 1 &&
				 (hid->quirks & HID_QUIRK_INPUT_PER_APP))
				hidinput = hidinput_match_application(report);

			if (!hidinput) {
				hidinput = hidinput_allocate(hid, application);
				if (!hidinput)
					goto out_unwind;
			}

			hidinput_configure_usages(hidinput, report);

			if (hid->quirks & HID_QUIRK_MULTI_INPUT)
				hidinput->report = report;

			list_add_tail(&report->hidinput_list,
				      &hidinput->reports);
		}
	}

	hidinput_change_resolution_multipliers(hid);

	list_for_each_entry_safe(hidinput, next, &hid->inputs, list) {
		if (drv->input_configured &&
		    drv->input_configured(hid, hidinput))
			goto out_unwind;

		if (!hidinput_has_been_populated(hidinput)) {
			/* no need to register an input device not populated */
			hidinput_cleanup_hidinput(hid, hidinput);
			continue;
		}

		if (input_register_device(hidinput->input))
			goto out_unwind;
		hidinput->registered = true;
	}

	if (list_empty(&hid->inputs)) {
		hid_err(hid, "No inputs registered, leaving\n");
		goto out_unwind;
	}

	if (hid->status & HID_STAT_DUP_DETECTED)
		hid_dbg(hid,
			"Some usages could not be mapped, please use HID_QUIRK_INCREMENT_USAGE_ON_DUPLICATE if this is legitimate.\n");

	return 0;

out_unwind:
	/* unwind the ones we already registered */
	hidinput_disconnect(hid);

	return -1;
}