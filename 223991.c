static void record_and_restart(struct perf_event *event, unsigned long val,
			       struct pt_regs *regs)
{
	u64 period = event->hw.sample_period;
	s64 prev, delta, left;
	int record = 0;

	if (event->hw.state & PERF_HES_STOPPED) {
		write_pmc(event->hw.idx, 0);
		return;
	}

	/* we don't have to worry about interrupts here */
	prev = local64_read(&event->hw.prev_count);
	delta = check_and_compute_delta(prev, val);
	local64_add(delta, &event->count);

	/*
	 * See if the total period for this event has expired,
	 * and update for the next period.
	 */
	val = 0;
	left = local64_read(&event->hw.period_left) - delta;
	if (delta == 0)
		left++;
	if (period) {
		if (left <= 0) {
			left += period;
			if (left <= 0)
				left = period;

			/*
			 * If address is not requested in the sample via
			 * PERF_SAMPLE_IP, just record that sample irrespective
			 * of SIAR valid check.
			 */
			if (event->attr.sample_type & PERF_SAMPLE_IP)
				record = siar_valid(regs);
			else
				record = 1;

			event->hw.last_period = event->hw.sample_period;
		}
		if (left < 0x80000000LL)
			val = 0x80000000LL - left;
	}

	write_pmc(event->hw.idx, val);
	local64_set(&event->hw.prev_count, val);
	local64_set(&event->hw.period_left, left);
	perf_event_update_userpage(event);

	/*
	 * Due to hardware limitation, sometimes SIAR could sample a kernel
	 * address even when freeze on supervisor state (kernel) is set in
	 * MMCR2. Check attr.exclude_kernel and address to drop the sample in
	 * these cases.
	 */
	if (event->attr.exclude_kernel &&
	    (event->attr.sample_type & PERF_SAMPLE_IP) &&
	    is_kernel_addr(mfspr(SPRN_SIAR)))
		record = 0;

	/*
	 * Finally record data if requested.
	 */
	if (record) {
		struct perf_sample_data data;

		perf_sample_data_init(&data, ~0ULL, event->hw.last_period);

		if (event->attr.sample_type & PERF_SAMPLE_ADDR_TYPE)
			perf_get_data_addr(event, regs, &data.addr);

		if (event->attr.sample_type & PERF_SAMPLE_BRANCH_STACK) {
			struct cpu_hw_events *cpuhw;
			cpuhw = this_cpu_ptr(&cpu_hw_events);
			power_pmu_bhrb_read(event, cpuhw);
			data.br_stack = &cpuhw->bhrb_stack;
		}

		if (event->attr.sample_type & PERF_SAMPLE_DATA_SRC &&
						ppmu->get_mem_data_src)
			ppmu->get_mem_data_src(&data.data_src, ppmu->flags, regs);

		if (event->attr.sample_type & PERF_SAMPLE_WEIGHT_TYPE &&
						ppmu->get_mem_weight)
			ppmu->get_mem_weight(&data.weight.full, event->attr.sample_type);

		if (perf_event_overflow(event, &data, regs))
			power_pmu_stop(event, 0);
	} else if (period) {
		/* Account for interrupt in case of invalid SIAR */
		if (perf_event_account_interrupt(event))
			power_pmu_stop(event, 0);
	}
}