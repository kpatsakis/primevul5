static bool mt_need_to_apply_feature(struct hid_device *hdev,
				     struct hid_field *field,
				     struct hid_usage *usage,
				     enum latency_mode latency,
				     bool surface_switch,
				     bool button_switch,
				     bool *inputmode_found)
{
	struct mt_device *td = hid_get_drvdata(hdev);
	struct mt_class *cls = &td->mtclass;
	struct hid_report *report = field->report;
	unsigned int index = usage->usage_index;
	char *buf;
	u32 report_len;
	int max;

	switch (usage->hid) {
	case HID_DG_INPUTMODE:
		/*
		 * Some elan panels wrongly declare 2 input mode features,
		 * and silently ignore when we set the value in the second
		 * field. Skip the second feature and hope for the best.
		 */
		if (*inputmode_found)
			return false;

		if (cls->quirks & MT_QUIRK_FORCE_GET_FEATURE) {
			report_len = hid_report_len(report);
			buf = hid_alloc_report_buf(report, GFP_KERNEL);
			if (!buf) {
				hid_err(hdev,
					"failed to allocate buffer for report\n");
				return false;
			}
			hid_hw_raw_request(hdev, report->id, buf, report_len,
					   HID_FEATURE_REPORT,
					   HID_REQ_GET_REPORT);
			kfree(buf);
		}

		field->value[index] = td->inputmode_value;
		*inputmode_found = true;
		return true;

	case HID_DG_CONTACTMAX:
		if (cls->maxcontacts) {
			max = min_t(int, field->logical_maximum,
				    cls->maxcontacts);
			if (field->value[index] != max) {
				field->value[index] = max;
				return true;
			}
		}
		break;

	case HID_DG_LATENCYMODE:
		field->value[index] = latency;
		return true;

	case HID_DG_SURFACESWITCH:
		field->value[index] = surface_switch;
		return true;

	case HID_DG_BUTTONSWITCH:
		field->value[index] = button_switch;
		return true;
	}

	return false; /* no need to update the report */
}