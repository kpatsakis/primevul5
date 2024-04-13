static void perf_event_interrupt(struct pt_regs *regs)
{
	u64 start_clock = sched_clock();

	__perf_event_interrupt(regs);
	perf_sample_event_took(sched_clock() - start_clock);
}