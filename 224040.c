static void write_mmcr0(struct cpu_hw_events *cpuhw, unsigned long mmcr0)
{
	unsigned long pmc5, pmc6;

	if (!cpuhw->n_limited) {
		mtspr(SPRN_MMCR0, mmcr0);
		return;
	}

	/*
	 * Write MMCR0, then read PMC5 and PMC6 immediately.
	 * To ensure we don't get a performance monitor interrupt
	 * between writing MMCR0 and freezing/thawing the limited
	 * events, we first write MMCR0 with the event overflow
	 * interrupt enable bits turned off.
	 */
	asm volatile("mtspr %3,%2; mfspr %0,%4; mfspr %1,%5"
		     : "=&r" (pmc5), "=&r" (pmc6)
		     : "r" (mmcr0 & ~(MMCR0_PMC1CE | MMCR0_PMCjCE)),
		       "i" (SPRN_MMCR0),
		       "i" (SPRN_PMC5), "i" (SPRN_PMC6));

	if (mmcr0 & MMCR0_FC)
		freeze_limited_counters(cpuhw, pmc5, pmc6);
	else
		thaw_limited_counters(cpuhw, pmc5, pmc6);

	/*
	 * Write the full MMCR0 including the event overflow interrupt
	 * enable bits, if necessary.
	 */
	if (mmcr0 & (MMCR0_PMC1CE | MMCR0_PMCjCE))
		mtspr(SPRN_MMCR0, mmcr0);
}