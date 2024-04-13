static void power_pmu_sched_task(struct perf_event_context *ctx, bool sched_in)
{
	if (!ppmu->bhrb_nr)
		return;

	if (sched_in)
		power_pmu_bhrb_reset();
}