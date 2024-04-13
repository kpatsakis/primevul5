static inline unsigned long perf_ip_adjust(struct pt_regs *regs)
{
	unsigned long mmcra = regs->dsisr;

	if ((ppmu->flags & PPMU_HAS_SSLOT) && (mmcra & MMCRA_SAMPLE_ENABLE)) {
		unsigned long slot = (mmcra & MMCRA_SLOT) >> MMCRA_SLOT_SHIFT;
		if (slot > 1)
			return 4 * (slot - 1);
	}

	return 0;
}