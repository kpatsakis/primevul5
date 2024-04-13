static inline void perf_get_data_addr(struct perf_event *event, struct pt_regs *regs, u64 *addrp)
{
	unsigned long mmcra = regs->dsisr;
	bool sdar_valid;

	if (ppmu->flags & PPMU_HAS_SIER)
		sdar_valid = regs->dar & SIER_SDAR_VALID;
	else {
		unsigned long sdsync;

		if (ppmu->flags & PPMU_SIAR_VALID)
			sdsync = POWER7P_MMCRA_SDAR_VALID;
		else if (ppmu->flags & PPMU_ALT_SIPR)
			sdsync = POWER6_MMCRA_SDSYNC;
		else if (ppmu->flags & PPMU_NO_SIAR)
			sdsync = MMCRA_SAMPLE_ENABLE;
		else
			sdsync = MMCRA_SDSYNC;

		sdar_valid = mmcra & sdsync;
	}

	if (!(mmcra & MMCRA_SAMPLE_ENABLE) || sdar_valid)
		*addrp = mfspr(SPRN_SDAR);

	if (is_kernel_addr(mfspr(SPRN_SDAR)) && event->attr.exclude_kernel)
		*addrp = 0;
}