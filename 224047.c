static void pmao_restore_workaround(bool ebb)
{
	unsigned pmcs[6];

	if (!cpu_has_feature(CPU_FTR_PMAO_BUG))
		return;

	/*
	 * On POWER8E there is a hardware defect which affects the PMU context
	 * switch logic, ie. power_pmu_disable/enable().
	 *
	 * When a counter overflows PMXE is cleared and FC/PMAO is set in MMCR0
	 * by the hardware. Sometime later the actual PMU exception is
	 * delivered.
	 *
	 * If we context switch, or simply disable/enable, the PMU prior to the
	 * exception arriving, the exception will be lost when we clear PMAO.
	 *
	 * When we reenable the PMU, we will write the saved MMCR0 with PMAO
	 * set, and this _should_ generate an exception. However because of the
	 * defect no exception is generated when we write PMAO, and we get
	 * stuck with no counters counting but no exception delivered.
	 *
	 * The workaround is to detect this case and tweak the hardware to
	 * create another pending PMU exception.
	 *
	 * We do that by setting up PMC6 (cycles) for an imminent overflow and
	 * enabling the PMU. That causes a new exception to be generated in the
	 * chip, but we don't take it yet because we have interrupts hard
	 * disabled. We then write back the PMU state as we want it to be seen
	 * by the exception handler. When we reenable interrupts the exception
	 * handler will be called and see the correct state.
	 *
	 * The logic is the same for EBB, except that the exception is gated by
	 * us having interrupts hard disabled as well as the fact that we are
	 * not in userspace. The exception is finally delivered when we return
	 * to userspace.
	 */

	/* Only if PMAO is set and PMAO_SYNC is clear */
	if ((current->thread.mmcr0 & (MMCR0_PMAO | MMCR0_PMAO_SYNC)) != MMCR0_PMAO)
		return;

	/* If we're doing EBB, only if BESCR[GE] is set */
	if (ebb && !(current->thread.bescr & BESCR_GE))
		return;

	/*
	 * We are already soft-disabled in power_pmu_enable(). We need to hard
	 * disable to actually prevent the PMU exception from firing.
	 */
	hard_irq_disable();

	/*
	 * This is a bit gross, but we know we're on POWER8E and have 6 PMCs.
	 * Using read/write_pmc() in a for loop adds 12 function calls and
	 * almost doubles our code size.
	 */
	pmcs[0] = mfspr(SPRN_PMC1);
	pmcs[1] = mfspr(SPRN_PMC2);
	pmcs[2] = mfspr(SPRN_PMC3);
	pmcs[3] = mfspr(SPRN_PMC4);
	pmcs[4] = mfspr(SPRN_PMC5);
	pmcs[5] = mfspr(SPRN_PMC6);

	/* Ensure all freeze bits are unset */
	mtspr(SPRN_MMCR2, 0);

	/* Set up PMC6 to overflow in one cycle */
	mtspr(SPRN_PMC6, 0x7FFFFFFE);

	/* Enable exceptions and unfreeze PMC6 */
	mtspr(SPRN_MMCR0, MMCR0_PMXE | MMCR0_PMCjCE | MMCR0_PMAO);

	/* Now we need to refreeze and restore the PMCs */
	mtspr(SPRN_MMCR0, MMCR0_FC | MMCR0_PMAO);

	mtspr(SPRN_PMC1, pmcs[0]);
	mtspr(SPRN_PMC2, pmcs[1]);
	mtspr(SPRN_PMC3, pmcs[2]);
	mtspr(SPRN_PMC4, pmcs[3]);
	mtspr(SPRN_PMC5, pmcs[4]);
	mtspr(SPRN_PMC6, pmcs[5]);
}