static bool is_ebb_event(struct perf_event *event)
{
	/*
	 * This could be a per-PMU callback, but we'd rather avoid the cost. We
	 * check that the PMU supports EBB, meaning those that don't can still
	 * use bit 63 of the event code for something else if they wish.
	 */
	return (ppmu->flags & PPMU_ARCH_207S) &&
	       ((event->attr.config >> PERF_EVENT_CONFIG_EBB_SHIFT) & 1);
}