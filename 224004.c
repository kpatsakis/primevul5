static void __perf_event_interrupt(struct pt_regs *regs)
{
	int i, j;
	struct cpu_hw_events *cpuhw = this_cpu_ptr(&cpu_hw_events);
	struct perf_event *event;
	int found, active;

	if (cpuhw->n_limited)
		freeze_limited_counters(cpuhw, mfspr(SPRN_PMC5),
					mfspr(SPRN_PMC6));

	perf_read_regs(regs);

	/* Read all the PMCs since we'll need them a bunch of times */
	for (i = 0; i < ppmu->n_counter; ++i)
		cpuhw->pmcs[i] = read_pmc(i + 1);

	/* Try to find what caused the IRQ */
	found = 0;
	for (i = 0; i < ppmu->n_counter; ++i) {
		if (!pmc_overflow(cpuhw->pmcs[i]))
			continue;
		if (is_limited_pmc(i + 1))
			continue; /* these won't generate IRQs */
		/*
		 * We've found one that's overflowed.  For active
		 * counters we need to log this.  For inactive
		 * counters, we need to reset it anyway
		 */
		found = 1;
		active = 0;
		for (j = 0; j < cpuhw->n_events; ++j) {
			event = cpuhw->event[j];
			if (event->hw.idx == (i + 1)) {
				active = 1;
				record_and_restart(event, cpuhw->pmcs[i], regs);
				break;
			}
		}
		if (!active)
			/* reset non active counters that have overflowed */
			write_pmc(i + 1, 0);
	}
	if (!found && pvr_version_is(PVR_POWER7)) {
		/* check active counters for special buggy p7 overflow */
		for (i = 0; i < cpuhw->n_events; ++i) {
			event = cpuhw->event[i];
			if (!event->hw.idx || is_limited_pmc(event->hw.idx))
				continue;
			if (pmc_overflow_power7(cpuhw->pmcs[event->hw.idx - 1])) {
				/* event has overflowed in a buggy way*/
				found = 1;
				record_and_restart(event,
						   cpuhw->pmcs[event->hw.idx - 1],
						   regs);
			}
		}
	}
	if (unlikely(!found) && !arch_irq_disabled_regs(regs))
		printk_ratelimited(KERN_WARNING "Can't find PMC that caused IRQ\n");

	/*
	 * Reset MMCR0 to its normal value.  This will set PMXE and
	 * clear FC (freeze counters) and PMAO (perf mon alert occurred)
	 * and thus allow interrupts to occur again.
	 * XXX might want to use MSR.PM to keep the events frozen until
	 * we get back out of this interrupt.
	 */
	write_mmcr0(cpuhw, cpuhw->mmcr.mmcr0);

	/* Clear the cpuhw->pmcs */
	memset(&cpuhw->pmcs, 0, sizeof(cpuhw->pmcs));

}