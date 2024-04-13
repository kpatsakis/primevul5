static void power_pmu_del(struct perf_event *event, int ef_flags)
{
	struct cpu_hw_events *cpuhw;
	long i;
	unsigned long flags;

	local_irq_save(flags);
	perf_pmu_disable(event->pmu);

	power_pmu_read(event);

	cpuhw = this_cpu_ptr(&cpu_hw_events);
	for (i = 0; i < cpuhw->n_events; ++i) {
		if (event == cpuhw->event[i]) {
			while (++i < cpuhw->n_events) {
				cpuhw->event[i-1] = cpuhw->event[i];
				cpuhw->events[i-1] = cpuhw->events[i];
				cpuhw->flags[i-1] = cpuhw->flags[i];
			}
			--cpuhw->n_events;
			ppmu->disable_pmc(event->hw.idx - 1, &cpuhw->mmcr);
			if (event->hw.idx) {
				write_pmc(event->hw.idx, 0);
				event->hw.idx = 0;
			}
			perf_event_update_userpage(event);
			break;
		}
	}
	for (i = 0; i < cpuhw->n_limited; ++i)
		if (event == cpuhw->limited_counter[i])
			break;
	if (i < cpuhw->n_limited) {
		while (++i < cpuhw->n_limited) {
			cpuhw->limited_counter[i-1] = cpuhw->limited_counter[i];
			cpuhw->limited_hwidx[i-1] = cpuhw->limited_hwidx[i];
		}
		--cpuhw->n_limited;
	}
	if (cpuhw->n_events == 0) {
		/* disable exceptions if no events are running */
		cpuhw->mmcr.mmcr0 &= ~(MMCR0_PMXE | MMCR0_FCECE);
	}

	if (has_branch_stack(event))
		power_pmu_bhrb_disable(event);

	perf_pmu_enable(event->pmu);
	local_irq_restore(flags);
}