static unsigned long ebb_switch_in(bool ebb, struct cpu_hw_events *cpuhw)
{
	unsigned long mmcr0 = cpuhw->mmcr.mmcr0;

	if (!ebb)
		goto out;

	/* Enable EBB and read/write to all 6 PMCs and BHRB for userspace */
	mmcr0 |= MMCR0_EBE | MMCR0_BHRBA | MMCR0_PMCC_U6;

	/*
	 * Add any bits from the user MMCR0, FC or PMAO. This is compatible
	 * with pmao_restore_workaround() because we may add PMAO but we never
	 * clear it here.
	 */
	mmcr0 |= current->thread.mmcr0;

	/*
	 * Be careful not to set PMXE if userspace had it cleared. This is also
	 * compatible with pmao_restore_workaround() because it has already
	 * cleared PMXE and we leave PMAO alone.
	 */
	if (!(current->thread.mmcr0 & MMCR0_PMXE))
		mmcr0 &= ~MMCR0_PMXE;

	mtspr(SPRN_SIAR, current->thread.siar);
	mtspr(SPRN_SIER, current->thread.sier);
	mtspr(SPRN_SDAR, current->thread.sdar);

	/*
	 * Merge the kernel & user values of MMCR2. The semantics we implement
	 * are that the user MMCR2 can set bits, ie. cause counters to freeze,
	 * but not clear bits. If a task wants to be able to clear bits, ie.
	 * unfreeze counters, it should not set exclude_xxx in its events and
	 * instead manage the MMCR2 entirely by itself.
	 */
	mtspr(SPRN_MMCR2, cpuhw->mmcr.mmcr2 | current->thread.mmcr2);

	if (ppmu->flags & PPMU_ARCH_31) {
		mtspr(SPRN_MMCR3, current->thread.mmcr3);
		mtspr(SPRN_SIER2, current->thread.sier2);
		mtspr(SPRN_SIER3, current->thread.sier3);
	}
out:
	return mmcr0;
}