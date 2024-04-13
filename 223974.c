static void ebb_switch_out(unsigned long mmcr0)
{
	if (!(mmcr0 & MMCR0_EBE))
		return;

	current->thread.siar  = mfspr(SPRN_SIAR);
	current->thread.sier  = mfspr(SPRN_SIER);
	current->thread.sdar  = mfspr(SPRN_SDAR);
	current->thread.mmcr0 = mmcr0 & MMCR0_USER_MASK;
	current->thread.mmcr2 = mfspr(SPRN_MMCR2) & MMCR2_USER_MASK;
	if (ppmu->flags & PPMU_ARCH_31) {
		current->thread.mmcr3 = mfspr(SPRN_MMCR3);
		current->thread.sier2 = mfspr(SPRN_SIER2);
		current->thread.sier3 = mfspr(SPRN_SIER3);
	}
}