static void power_pmu_bhrb_enable(struct perf_event *event)
{
	struct cpu_hw_events *cpuhw = this_cpu_ptr(&cpu_hw_events);

	if (!ppmu->bhrb_nr)
		return;

	/* Clear BHRB if we changed task context to avoid data leaks */
	if (event->ctx->task && cpuhw->bhrb_context != event->ctx) {
		power_pmu_bhrb_reset();
		cpuhw->bhrb_context = event->ctx;
	}
	cpuhw->bhrb_users++;
	perf_sched_cb_inc(event->ctx->pmu);
}