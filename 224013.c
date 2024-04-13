unsigned long get_pmcs_ext_regs(int idx)
{
	struct cpu_hw_events *cpuhw = this_cpu_ptr(&cpu_hw_events);

	return cpuhw->pmcs[idx];
}