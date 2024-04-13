static void hidinput_change_resolution_multipliers(struct hid_device *hid)
{
	struct hid_report_enum *rep_enum;
	struct hid_report *rep;
	int ret;

	rep_enum = &hid->report_enum[HID_FEATURE_REPORT];
	list_for_each_entry(rep, &rep_enum->report_list, list) {
		bool update_needed = __hidinput_change_resolution_multipliers(hid,
								     rep, true);

		if (update_needed) {
			ret = __hid_request(hid, rep, HID_REQ_SET_REPORT);
			if (ret) {
				__hidinput_change_resolution_multipliers(hid,
								    rep, false);
				return;
			}
		}
	}

	/* refresh our structs */
	hid_setup_resolution_multiplier(hid);
}