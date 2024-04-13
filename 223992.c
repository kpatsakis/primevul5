static int power_pmu_event_init(struct perf_event *event)
{
	u64 ev;
	unsigned long flags, irq_flags;
	struct perf_event *ctrs[MAX_HWEVENTS];
	u64 events[MAX_HWEVENTS];
	unsigned int cflags[MAX_HWEVENTS];
	int n;
	int err;
	struct cpu_hw_events *cpuhw;

	if (!ppmu)
		return -ENOENT;

	if (has_branch_stack(event)) {
	        /* PMU has BHRB enabled */
		if (!(ppmu->flags & PPMU_ARCH_207S))
			return -EOPNOTSUPP;
	}

	switch (event->attr.type) {
	case PERF_TYPE_HARDWARE:
		ev = event->attr.config;
		if (ev >= ppmu->n_generic || ppmu->generic_events[ev] == 0)
			return -EOPNOTSUPP;

		if (ppmu->blacklist_ev && is_event_blacklisted(ev))
			return -EINVAL;
		ev = ppmu->generic_events[ev];
		break;
	case PERF_TYPE_HW_CACHE:
		err = hw_perf_cache_event(event->attr.config, &ev);
		if (err)
			return err;

		if (ppmu->blacklist_ev && is_event_blacklisted(ev))
			return -EINVAL;
		break;
	case PERF_TYPE_RAW:
		ev = event->attr.config;

		if (ppmu->blacklist_ev && is_event_blacklisted(ev))
			return -EINVAL;
		break;
	default:
		return -ENOENT;
	}

	/*
	 * PMU config registers have fields that are
	 * reserved and some specific values for bit fields are reserved.
	 * For ex., MMCRA[61:62] is Randome Sampling Mode (SM)
	 * and value of 0b11 to this field is reserved.
	 * Check for invalid values in attr.config.
	 */
	if (ppmu->check_attr_config &&
	    ppmu->check_attr_config(event))
		return -EINVAL;

	event->hw.config_base = ev;
	event->hw.idx = 0;

	/*
	 * If we are not running on a hypervisor, force the
	 * exclude_hv bit to 0 so that we don't care what
	 * the user set it to.
	 */
	if (!firmware_has_feature(FW_FEATURE_LPAR))
		event->attr.exclude_hv = 0;

	/*
	 * If this is a per-task event, then we can use
	 * PM_RUN_* events interchangeably with their non RUN_*
	 * equivalents, e.g. PM_RUN_CYC instead of PM_CYC.
	 * XXX we should check if the task is an idle task.
	 */
	flags = 0;
	if (event->attach_state & PERF_ATTACH_TASK)
		flags |= PPMU_ONLY_COUNT_RUN;

	/*
	 * If this machine has limited events, check whether this
	 * event_id could go on a limited event.
	 */
	if (ppmu->flags & PPMU_LIMITED_PMC5_6) {
		if (can_go_on_limited_pmc(event, ev, flags)) {
			flags |= PPMU_LIMITED_PMC_OK;
		} else if (ppmu->limited_pmc_event(ev)) {
			/*
			 * The requested event_id is on a limited PMC,
			 * but we can't use a limited PMC; see if any
			 * alternative goes on a normal PMC.
			 */
			ev = normal_pmc_alternative(ev, flags);
			if (!ev)
				return -EINVAL;
		}
	}

	/* Extra checks for EBB */
	err = ebb_event_check(event);
	if (err)
		return err;

	/*
	 * If this is in a group, check if it can go on with all the
	 * other hardware events in the group.  We assume the event
	 * hasn't been linked into its leader's sibling list at this point.
	 */
	n = 0;
	if (event->group_leader != event) {
		n = collect_events(event->group_leader, ppmu->n_counter - 1,
				   ctrs, events, cflags);
		if (n < 0)
			return -EINVAL;
	}
	events[n] = ev;
	ctrs[n] = event;
	cflags[n] = flags;
	if (check_excludes(ctrs, cflags, n, 1))
		return -EINVAL;

	local_irq_save(irq_flags);
	cpuhw = this_cpu_ptr(&cpu_hw_events);

	err = power_check_constraints(cpuhw, events, cflags, n + 1, ctrs);

	if (has_branch_stack(event)) {
		u64 bhrb_filter = -1;

		if (ppmu->bhrb_filter_map)
			bhrb_filter = ppmu->bhrb_filter_map(
					event->attr.branch_sample_type);

		if (bhrb_filter == -1) {
			local_irq_restore(irq_flags);
			return -EOPNOTSUPP;
		}
		cpuhw->bhrb_filter = bhrb_filter;
	}

	local_irq_restore(irq_flags);
	if (err)
		return -EINVAL;

	event->hw.config = events[n];
	event->hw.event_base = cflags[n];
	event->hw.last_period = event->hw.sample_period;
	local64_set(&event->hw.period_left, event->hw.last_period);

	/*
	 * For EBB events we just context switch the PMC value, we don't do any
	 * of the sample_period logic. We use hw.prev_count for this.
	 */
	if (is_ebb_event(event))
		local64_set(&event->hw.prev_count, 0);

	/*
	 * See if we need to reserve the PMU.
	 * If no events are currently in use, then we have to take a
	 * mutex to ensure that we don't race with another task doing
	 * reserve_pmc_hardware or release_pmc_hardware.
	 */
	err = 0;
	if (!atomic_inc_not_zero(&num_events)) {
		mutex_lock(&pmc_reserve_mutex);
		if (atomic_read(&num_events) == 0 &&
		    reserve_pmc_hardware(perf_event_interrupt))
			err = -EBUSY;
		else
			atomic_inc(&num_events);
		mutex_unlock(&pmc_reserve_mutex);
	}
	event->destroy = hw_perf_event_destroy;

	return err;
}