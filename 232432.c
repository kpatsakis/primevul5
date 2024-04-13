static struct hid_usage *hidinput_find_key(struct hid_device *hid,
					   hid_usage_cmp_t match,
					   unsigned int value,
					   unsigned int *usage_idx)
{
	unsigned int i, j, k, cur_idx = 0;
	struct hid_report *report;
	struct hid_usage *usage;

	for (k = HID_INPUT_REPORT; k <= HID_OUTPUT_REPORT; k++) {
		list_for_each_entry(report, &hid->report_enum[k].report_list, list) {
			for (i = 0; i < report->maxfield; i++) {
				for (j = 0; j < report->field[i]->maxusage; j++) {
					usage = report->field[i]->usage + j;
					if (usage->type == EV_KEY || usage->type == 0) {
						if (match(usage, cur_idx, value)) {
							if (usage_idx)
								*usage_idx = cur_idx;
							return usage;
						}
						cur_idx++;
					}
				}
			}
		}
	}
	return NULL;
}