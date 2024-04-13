static inline u32 perf_flags_from_msr(struct pt_regs *regs)
{
	if (regs->msr & MSR_PR)
		return PERF_RECORD_MISC_USER;
	if ((regs->msr & MSR_HV) && freeze_events_kernel != MMCR0_FCHV)
		return PERF_RECORD_MISC_HYPERVISOR;
	return PERF_RECORD_MISC_KERNEL;
}