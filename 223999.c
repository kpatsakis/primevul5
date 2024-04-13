static inline u32 perf_get_misc_flags(struct pt_regs *regs)
{
	bool use_siar = regs_use_siar(regs);
	unsigned long mmcra = regs->dsisr;
	int marked = mmcra & MMCRA_SAMPLE_ENABLE;

	if (!use_siar)
		return perf_flags_from_msr(regs);

	/*
	 * Check the address in SIAR to identify the
	 * privilege levels since the SIER[MSR_HV, MSR_PR]
	 * bits are not set for marked events in power10
	 * DD1.
	 */
	if (marked && (ppmu->flags & PPMU_P10_DD1)) {
		unsigned long siar = mfspr(SPRN_SIAR);
		if (siar) {
			if (is_kernel_addr(siar))
				return PERF_RECORD_MISC_KERNEL;
			return PERF_RECORD_MISC_USER;
		} else {
			if (is_kernel_addr(regs->nip))
				return PERF_RECORD_MISC_KERNEL;
			return PERF_RECORD_MISC_USER;
		}
	}

	/*
	 * If we don't have flags in MMCRA, rather than using
	 * the MSR, we intuit the flags from the address in
	 * SIAR which should give slightly more reliable
	 * results
	 */
	if (ppmu->flags & PPMU_NO_SIPR) {
		unsigned long siar = mfspr(SPRN_SIAR);
		if (is_kernel_addr(siar))
			return PERF_RECORD_MISC_KERNEL;
		return PERF_RECORD_MISC_USER;
	}

	/* PR has priority over HV, so order below is important */
	if (regs_sipr(regs))
		return PERF_RECORD_MISC_USER;

	if (regs_sihv(regs) && (freeze_events_kernel != MMCR0_FCHV))
		return PERF_RECORD_MISC_HYPERVISOR;

	return PERF_RECORD_MISC_KERNEL;
}