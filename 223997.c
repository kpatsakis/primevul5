static inline int siar_valid(struct pt_regs *regs)
{
	unsigned long mmcra = regs->dsisr;
	int marked = mmcra & MMCRA_SAMPLE_ENABLE;

	if (marked) {
		/*
		 * SIER[SIAR_VALID] is not set for some
		 * marked events on power10 DD1, so drop
		 * the check for SIER[SIAR_VALID] and return true.
		 */
		if (ppmu->flags & PPMU_P10_DD1)
			return 0x1;
		else if (ppmu->flags & PPMU_HAS_SIER)
			return regs->dar & SIER_SIAR_VALID;

		if (ppmu->flags & PPMU_SIAR_VALID)
			return mmcra & POWER7P_MMCRA_SIAR_VALID;
	}

	return 1;
}