void perf_event_print_debug(void)
{
	unsigned long sdar, sier, flags;
	u32 pmcs[MAX_HWEVENTS];
	int i;

	if (!ppmu) {
		pr_info("Performance monitor hardware not registered.\n");
		return;
	}

	if (!ppmu->n_counter)
		return;

	local_irq_save(flags);

	pr_info("CPU: %d PMU registers, ppmu = %s n_counters = %d",
		 smp_processor_id(), ppmu->name, ppmu->n_counter);

	for (i = 0; i < ppmu->n_counter; i++)
		pmcs[i] = read_pmc(i + 1);

	for (; i < MAX_HWEVENTS; i++)
		pmcs[i] = 0xdeadbeef;

	pr_info("PMC1:  %08x PMC2: %08x PMC3: %08x PMC4: %08x\n",
		 pmcs[0], pmcs[1], pmcs[2], pmcs[3]);

	if (ppmu->n_counter > 4)
		pr_info("PMC5:  %08x PMC6: %08x PMC7: %08x PMC8: %08x\n",
			 pmcs[4], pmcs[5], pmcs[6], pmcs[7]);

	pr_info("MMCR0: %016lx MMCR1: %016lx MMCRA: %016lx\n",
		mfspr(SPRN_MMCR0), mfspr(SPRN_MMCR1), mfspr(SPRN_MMCRA));

	sdar = sier = 0;
#ifdef CONFIG_PPC64
	sdar = mfspr(SPRN_SDAR);

	if (ppmu->flags & PPMU_HAS_SIER)
		sier = mfspr(SPRN_SIER);

	if (ppmu->flags & PPMU_ARCH_207S) {
		pr_info("MMCR2: %016lx EBBHR: %016lx\n",
			mfspr(SPRN_MMCR2), mfspr(SPRN_EBBHR));
		pr_info("EBBRR: %016lx BESCR: %016lx\n",
			mfspr(SPRN_EBBRR), mfspr(SPRN_BESCR));
	}

	if (ppmu->flags & PPMU_ARCH_31) {
		pr_info("MMCR3: %016lx SIER2: %016lx SIER3: %016lx\n",
			mfspr(SPRN_MMCR3), mfspr(SPRN_SIER2), mfspr(SPRN_SIER3));
	}
#endif
	pr_info("SIAR:  %016lx SDAR:  %016lx SIER:  %016lx\n",
		mfspr(SPRN_SIAR), sdar, sier);

	local_irq_restore(flags);
}