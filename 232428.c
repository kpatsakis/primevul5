static inline void hidinput_configure_usages(struct hid_input *hidinput,
					     struct hid_report *report)
{
	int i, j;

	for (i = 0; i < report->maxfield; i++)
		for (j = 0; j < report->field[i]->maxusage; j++)
			hidinput_configure_usage(hidinput, report->field[i],
						 report->field[i]->usage + j);
}