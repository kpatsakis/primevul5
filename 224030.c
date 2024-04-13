static bool regs_sihv(struct pt_regs *regs)
{
	unsigned long sihv = MMCRA_SIHV;

	if (ppmu->flags & PPMU_HAS_SIER)
		return !!(regs->dar & SIER_SIHV);

	if (ppmu->flags & PPMU_ALT_SIPR)
		sihv = POWER6_MMCRA_SIHV;

	return !!(regs->dsisr & sihv);
}