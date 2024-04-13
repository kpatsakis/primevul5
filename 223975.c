static int can_go_on_limited_pmc(struct perf_event *event, u64 ev,
				 unsigned int flags)
{
	int n;
	u64 alt[MAX_EVENT_ALTERNATIVES];

	if (event->attr.exclude_user
	    || event->attr.exclude_kernel
	    || event->attr.exclude_hv
	    || event->attr.sample_period)
		return 0;

	if (ppmu->limited_pmc_event(ev))
		return 1;

	/*
	 * The requested event_id isn't on a limited PMC already;
	 * see if any alternative code goes on a limited PMC.
	 */
	if (!ppmu->get_alternatives)
		return 0;

	flags |= PPMU_LIMITED_PMC_OK | PPMU_LIMITED_PMC_REQD;
	n = ppmu->get_alternatives(ev, flags, alt);

	return n > 0;
}