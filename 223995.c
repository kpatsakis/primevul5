static void freeze_limited_counters(struct cpu_hw_events *cpuhw,
				    unsigned long pmc5, unsigned long pmc6)
{
	struct perf_event *event;
	u64 val, prev, delta;
	int i;

	for (i = 0; i < cpuhw->n_limited; ++i) {
		event = cpuhw->limited_counter[i];
		if (!event->hw.idx)
			continue;
		val = (event->hw.idx == 5) ? pmc5 : pmc6;
		prev = local64_read(&event->hw.prev_count);
		event->hw.idx = 0;
		delta = check_and_compute_delta(prev, val);
		if (delta)
			local64_add(delta, &event->count);
	}
}