static void power_pmu_stop(struct perf_event *event, int ef_flags)
{
	unsigned long flags;

	if (!event->hw.idx || !event->hw.sample_period)
		return;

	if (event->hw.state & PERF_HES_STOPPED)
		return;

	local_irq_save(flags);
	perf_pmu_disable(event->pmu);

	power_pmu_read(event);
	event->hw.state |= PERF_HES_STOPPED | PERF_HES_UPTODATE;
	write_pmc(event->hw.idx, 0);

	perf_event_update_userpage(event);
	perf_pmu_enable(event->pmu);
	local_irq_restore(flags);
}