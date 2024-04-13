static unsigned long ebb_switch_in(bool ebb, struct cpu_hw_events *cpuhw)
{
	return cpuhw->mmcr.mmcr0;
}