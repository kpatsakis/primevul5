static int power_pmu_prepare_cpu(unsigned int cpu)
{
	struct cpu_hw_events *cpuhw = &per_cpu(cpu_hw_events, cpu);

	if (ppmu) {
		memset(cpuhw, 0, sizeof(*cpuhw));
		cpuhw->mmcr.mmcr0 = MMCR0_FC;
	}
	return 0;
}