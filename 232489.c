static void report_features(struct hid_device *hid)
{
	struct hid_driver *drv = hid->driver;
	struct hid_report_enum *rep_enum;
	struct hid_report *rep;
	struct hid_usage *usage;
	int i, j;

	rep_enum = &hid->report_enum[HID_FEATURE_REPORT];
	list_for_each_entry(rep, &rep_enum->report_list, list)
		for (i = 0; i < rep->maxfield; i++) {
			/* Ignore if report count is out of bounds. */
			if (rep->field[i]->report_count < 1)
				continue;

			for (j = 0; j < rep->field[i]->maxusage; j++) {
				usage = &rep->field[i]->usage[j];

				/* Verify if Battery Strength feature is available */
				if (usage->hid == HID_DC_BATTERYSTRENGTH)
					hidinput_setup_battery(hid, HID_FEATURE_REPORT,
							       rep->field[i]);

				if (drv->feature_mapping)
					drv->feature_mapping(hid, rep->field[i], usage);
			}
		}
}