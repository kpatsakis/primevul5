static void power_pmu_start(struct perf_event *event, int ef_flags)
{
	unsigned long flags;
	s64 left;
	unsigned long val;

	if (!event->hw.idx || !event->hw.sample_period)
		return;

	if (!(event->hw.state & PERF_HES_STOPPED))
		return;

	if (ef_flags & PERF_EF_RELOAD)
		WARN_ON_ONCE(!(event->hw.state & PERF_HES_UPTODATE));

	local_irq_save(flags);
	perf_pmu_disable(event->pmu);

	event->hw.state = 0;
	left = local64_read(&event->hw.period_left);

	val = 0;
	if (left < 0x80000000L)
		val = 0x80000000L - left;

	write_pmc(event->hw.idx, val);

	perf_event_update_userpage(event);
	perf_pmu_enable(event->pmu);
	local_irq_restore(flags);
}