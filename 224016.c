static inline void perf_read_regs(struct pt_regs *regs)
{
	unsigned long mmcra = mfspr(SPRN_MMCRA);
	int marked = mmcra & MMCRA_SAMPLE_ENABLE;
	int use_siar;

	regs->dsisr = mmcra;

	if (ppmu->flags & PPMU_HAS_SIER)
		regs->dar = mfspr(SPRN_SIER);

	/*
	 * If this isn't a PMU exception (eg a software event) the SIAR is
	 * not valid. Use pt_regs.
	 *
	 * If it is a marked event use the SIAR.
	 *
	 * If the PMU doesn't update the SIAR for non marked events use
	 * pt_regs.
	 *
	 * If the PMU has HV/PR flags then check to see if they
	 * place the exception in userspace. If so, use pt_regs. In
	 * continuous sampling mode the SIAR and the PMU exception are
	 * not synchronised, so they may be many instructions apart.
	 * This can result in confusing backtraces. We still want
	 * hypervisor samples as well as samples in the kernel with
	 * interrupts off hence the userspace check.
	 */
	if (TRAP(regs) != INTERRUPT_PERFMON)
		use_siar = 0;
	else if ((ppmu->flags & PPMU_NO_SIAR))
		use_siar = 0;
	else if (marked)
		use_siar = 1;
	else if ((ppmu->flags & PPMU_NO_CONT_SAMPLING))
		use_siar = 0;
	else if (!(ppmu->flags & PPMU_NO_SIPR) && regs_sipr(regs))
		use_siar = 0;
	else
		use_siar = 1;

	regs->result = use_siar;
}