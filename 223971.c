static void power_pmu_disable(struct pmu *pmu)
{
	struct cpu_hw_events *cpuhw;
	unsigned long flags, mmcr0, val, mmcra;

	if (!ppmu)
		return;
	local_irq_save(flags);
	cpuhw = this_cpu_ptr(&cpu_hw_events);

	if (!cpuhw->disabled) {
		/*
		 * Check if we ever enabled the PMU on this cpu.
		 */
		if (!cpuhw->pmcs_enabled) {
			ppc_enable_pmcs();
			cpuhw->pmcs_enabled = 1;
		}

		/*
		 * Set the 'freeze counters' bit, clear EBE/BHRBA/PMCC/PMAO/FC56
		 */
		val  = mmcr0 = mfspr(SPRN_MMCR0);
		val |= MMCR0_FC;
		val &= ~(MMCR0_EBE | MMCR0_BHRBA | MMCR0_PMCC | MMCR0_PMAO |
			 MMCR0_FC56);
		/* Set mmcr0 PMCCEXT for p10 */
		if (ppmu->flags & PPMU_ARCH_31)
			val |= MMCR0_PMCCEXT;

		/*
		 * The barrier is to make sure the mtspr has been
		 * executed and the PMU has frozen the events etc.
		 * before we return.
		 */
		write_mmcr0(cpuhw, val);
		mb();
		isync();

		val = mmcra = cpuhw->mmcr.mmcra;

		/*
		 * Disable instruction sampling if it was enabled
		 */
		if (cpuhw->mmcr.mmcra & MMCRA_SAMPLE_ENABLE)
			val &= ~MMCRA_SAMPLE_ENABLE;

		/* Disable BHRB via mmcra (BHRBRD) for p10 */
		if (ppmu->flags & PPMU_ARCH_31)
			val |= MMCRA_BHRB_DISABLE;

		/*
		 * Write SPRN_MMCRA if mmcra has either disabled
		 * instruction sampling or BHRB.
		 */
		if (val != mmcra) {
			mtspr(SPRN_MMCRA, mmcra);
			mb();
			isync();
		}

		cpuhw->disabled = 1;
		cpuhw->n_added = 0;

		ebb_switch_out(mmcr0);

#ifdef CONFIG_PPC64
		/*
		 * These are readable by userspace, may contain kernel
		 * addresses and are not switched by context switch, so clear
		 * them now to avoid leaking anything to userspace in general
		 * including to another process.
		 */
		if (ppmu->flags & PPMU_ARCH_207S) {
			mtspr(SPRN_SDAR, 0);
			mtspr(SPRN_SIAR, 0);
		}
#endif
	}

	local_irq_restore(flags);
}