static void power_pmu_read(struct perf_event *event)
{
	s64 val, delta, prev;

	if (event->hw.state & PERF_HES_STOPPED)
		return;

	if (!event->hw.idx)
		return;

	if (is_ebb_event(event)) {
		val = read_pmc(event->hw.idx);
		local64_set(&event->hw.prev_count, val);
		return;
	}

	/*
	 * Performance monitor interrupts come even when interrupts
	 * are soft-disabled, as long as interrupts are hard-enabled.
	 * Therefore we treat them like NMIs.
	 */
	do {
		prev = local64_read(&event->hw.prev_count);
		barrier();
		val = read_pmc(event->hw.idx);
		delta = check_and_compute_delta(prev, val);
		if (!delta)
			return;
	} while (local64_cmpxchg(&event->hw.prev_count, prev, val) != prev);

	local64_add(delta, &event->count);

	/*
	 * A number of places program the PMC with (0x80000000 - period_left).
	 * We never want period_left to be less than 1 because we will program
	 * the PMC with a value >= 0x800000000 and an edge detected PMC will
	 * roll around to 0 before taking an exception. We have seen this
	 * on POWER8.
	 *
	 * To fix this, clamp the minimum value of period_left to 1.
	 */
	do {
		prev = local64_read(&event->hw.period_left);
		val = prev - delta;
		if (val < 1)
			val = 1;
	} while (local64_cmpxchg(&event->hw.period_left, prev, val) != prev);
}