static int power_pmu_add(struct perf_event *event, int ef_flags)
{
	struct cpu_hw_events *cpuhw;
	unsigned long flags;
	int n0;
	int ret = -EAGAIN;

	local_irq_save(flags);
	perf_pmu_disable(event->pmu);

	/*
	 * Add the event to the list (if there is room)
	 * and check whether the total set is still feasible.
	 */
	cpuhw = this_cpu_ptr(&cpu_hw_events);
	n0 = cpuhw->n_events;
	if (n0 >= ppmu->n_counter)
		goto out;
	cpuhw->event[n0] = event;
	cpuhw->events[n0] = event->hw.config;
	cpuhw->flags[n0] = event->hw.event_base;

	/*
	 * This event may have been disabled/stopped in record_and_restart()
	 * because we exceeded the ->event_limit. If re-starting the event,
	 * clear the ->hw.state (STOPPED and UPTODATE flags), so the user
	 * notification is re-enabled.
	 */
	if (!(ef_flags & PERF_EF_START))
		event->hw.state = PERF_HES_STOPPED | PERF_HES_UPTODATE;
	else
		event->hw.state = 0;

	/*
	 * If group events scheduling transaction was started,
	 * skip the schedulability test here, it will be performed
	 * at commit time(->commit_txn) as a whole
	 */
	if (cpuhw->txn_flags & PERF_PMU_TXN_ADD)
		goto nocheck;

	if (check_excludes(cpuhw->event, cpuhw->flags, n0, 1))
		goto out;
	if (power_check_constraints(cpuhw, cpuhw->events, cpuhw->flags, n0 + 1, cpuhw->event))
		goto out;
	event->hw.config = cpuhw->events[n0];

nocheck:
	ebb_event_add(event);

	++cpuhw->n_events;
	++cpuhw->n_added;

	ret = 0;
 out:
	if (has_branch_stack(event)) {
		u64 bhrb_filter = -1;

		if (ppmu->bhrb_filter_map)
			bhrb_filter = ppmu->bhrb_filter_map(
				event->attr.branch_sample_type);

		if (bhrb_filter != -1) {
			cpuhw->bhrb_filter = bhrb_filter;
			power_pmu_bhrb_enable(event);
		}
	}

	perf_pmu_enable(event->pmu);
	local_irq_restore(flags);
	return ret;
}