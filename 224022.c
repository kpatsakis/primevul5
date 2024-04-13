static bool regs_sipr(struct pt_regs *regs)
{
	unsigned long sipr = MMCRA_SIPR;

	if (ppmu->flags & PPMU_HAS_SIER)
		return !!(regs->dar & SIER_SIPR);

	if (ppmu->flags & PPMU_ALT_SIPR)
		sipr = POWER6_MMCRA_SIPR;

	return !!(regs->dsisr & sipr);
}