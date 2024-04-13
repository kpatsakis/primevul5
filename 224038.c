static void power_pmu_enable(struct pmu *pmu)
{
	struct perf_event *event;
	struct cpu_hw_events *cpuhw;
	unsigned long flags;
	long i;
	unsigned long val, mmcr0;
	s64 left;
	unsigned int hwc_index[MAX_HWEVENTS];
	int n_lim;
	int idx;
	bool ebb;

	if (!ppmu)
		return;
	local_irq_save(flags);

	cpuhw = this_cpu_ptr(&cpu_hw_events);
	if (!cpuhw->disabled)
		goto out;

	if (cpuhw->n_events == 0) {
		ppc_set_pmu_inuse(0);
		goto out;
	}

	cpuhw->disabled = 0;

	/*
	 * EBB requires an exclusive group and all events must have the EBB
	 * flag set, or not set, so we can just check a single event. Also we
	 * know we have at least one event.
	 */
	ebb = is_ebb_event(cpuhw->event[0]);

	/*
	 * If we didn't change anything, or only removed events,
	 * no need to recalculate MMCR* settings and reset the PMCs.
	 * Just reenable the PMU with the current MMCR* settings
	 * (possibly updated for removal of events).
	 */
	if (!cpuhw->n_added) {
		mtspr(SPRN_MMCRA, cpuhw->mmcr.mmcra & ~MMCRA_SAMPLE_ENABLE);
		mtspr(SPRN_MMCR1, cpuhw->mmcr.mmcr1);
		if (ppmu->flags & PPMU_ARCH_31)
			mtspr(SPRN_MMCR3, cpuhw->mmcr.mmcr3);
		goto out_enable;
	}

	/*
	 * Clear all MMCR settings and recompute them for the new set of events.
	 */
	memset(&cpuhw->mmcr, 0, sizeof(cpuhw->mmcr));

	if (ppmu->compute_mmcr(cpuhw->events, cpuhw->n_events, hwc_index,
			       &cpuhw->mmcr, cpuhw->event, ppmu->flags)) {
		/* shouldn't ever get here */
		printk(KERN_ERR "oops compute_mmcr failed\n");
		goto out;
	}

	if (!(ppmu->flags & PPMU_ARCH_207S)) {
		/*
		 * Add in MMCR0 freeze bits corresponding to the attr.exclude_*
		 * bits for the first event. We have already checked that all
		 * events have the same value for these bits as the first event.
		 */
		event = cpuhw->event[0];
		if (event->attr.exclude_user)
			cpuhw->mmcr.mmcr0 |= MMCR0_FCP;
		if (event->attr.exclude_kernel)
			cpuhw->mmcr.mmcr0 |= freeze_events_kernel;
		if (event->attr.exclude_hv)
			cpuhw->mmcr.mmcr0 |= MMCR0_FCHV;
	}

	/*
	 * Write the new configuration to MMCR* with the freeze
	 * bit set and set the hardware events to their initial values.
	 * Then unfreeze the events.
	 */
	ppc_set_pmu_inuse(1);
	mtspr(SPRN_MMCRA, cpuhw->mmcr.mmcra & ~MMCRA_SAMPLE_ENABLE);
	mtspr(SPRN_MMCR1, cpuhw->mmcr.mmcr1);
	mtspr(SPRN_MMCR0, (cpuhw->mmcr.mmcr0 & ~(MMCR0_PMC1CE | MMCR0_PMCjCE))
				| MMCR0_FC);
	if (ppmu->flags & PPMU_ARCH_207S)
		mtspr(SPRN_MMCR2, cpuhw->mmcr.mmcr2);

	if (ppmu->flags & PPMU_ARCH_31)
		mtspr(SPRN_MMCR3, cpuhw->mmcr.mmcr3);

	/*
	 * Read off any pre-existing events that need to move
	 * to another PMC.
	 */
	for (i = 0; i < cpuhw->n_events; ++i) {
		event = cpuhw->event[i];
		if (event->hw.idx && event->hw.idx != hwc_index[i] + 1) {
			power_pmu_read(event);
			write_pmc(event->hw.idx, 0);
			event->hw.idx = 0;
		}
	}

	/*
	 * Initialize the PMCs for all the new and moved events.
	 */
	cpuhw->n_limited = n_lim = 0;
	for (i = 0; i < cpuhw->n_events; ++i) {
		event = cpuhw->event[i];
		if (event->hw.idx)
			continue;
		idx = hwc_index[i] + 1;
		if (is_limited_pmc(idx)) {
			cpuhw->limited_counter[n_lim] = event;
			cpuhw->limited_hwidx[n_lim] = idx;
			++n_lim;
			continue;
		}

		if (ebb)
			val = local64_read(&event->hw.prev_count);
		else {
			val = 0;
			if (event->hw.sample_period) {
				left = local64_read(&event->hw.period_left);
				if (left < 0x80000000L)
					val = 0x80000000L - left;
			}
			local64_set(&event->hw.prev_count, val);
		}

		event->hw.idx = idx;
		if (event->hw.state & PERF_HES_STOPPED)
			val = 0;
		write_pmc(idx, val);

		perf_event_update_userpage(event);
	}
	cpuhw->n_limited = n_lim;
	cpuhw->mmcr.mmcr0 |= MMCR0_PMXE | MMCR0_FCECE;

 out_enable:
	pmao_restore_workaround(ebb);

	mmcr0 = ebb_switch_in(ebb, cpuhw);

	mb();
	if (cpuhw->bhrb_users)
		ppmu->config_bhrb(cpuhw->bhrb_filter);

	write_mmcr0(cpuhw, mmcr0);

	/*
	 * Enable instruction sampling if necessary
	 */
	if (cpuhw->mmcr.mmcra & MMCRA_SAMPLE_ENABLE) {
		mb();
		mtspr(SPRN_MMCRA, cpuhw->mmcr.mmcra);
	}

 out:

	local_irq_restore(flags);
}