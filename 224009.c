static void thaw_limited_counters(struct cpu_hw_events *cpuhw,
				  unsigned long pmc5, unsigned long pmc6)
{
	struct perf_event *event;
	u64 val, prev;
	int i;

	for (i = 0; i < cpuhw->n_limited; ++i) {
		event = cpuhw->limited_counter[i];
		event->hw.idx = cpuhw->limited_hwidx[i];
		val = (event->hw.idx == 5) ? pmc5 : pmc6;
		prev = local64_read(&event->hw.prev_count);
		if (check_and_compute_delta(prev, val))
			local64_set(&event->hw.prev_count, val);
		perf_event_update_userpage(event);
	}
}