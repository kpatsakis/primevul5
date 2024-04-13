static inline u32 hid_report_len(struct hid_report *report)
{
	/* equivalent to DIV_ROUND_UP(report->size, 8) + !!(report->id > 0) */
	return ((report->size - 1) >> 3) + 1 + (report->id > 0);
}