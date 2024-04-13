void trace_buffered_event_disable(void)
{
	int cpu;

	WARN_ON_ONCE(!mutex_is_locked(&event_mutex));

	if (WARN_ON_ONCE(!trace_buffered_event_ref))
		return;

	if (--trace_buffered_event_ref)
		return;

	preempt_disable();
	/* For each CPU, set the buffer as used. */
	smp_call_function_many(tracing_buffer_mask,
			       disable_trace_buffered_event, NULL, 1);
	preempt_enable();

	/* Wait for all current users to finish */
	synchronize_sched();

	for_each_tracing_cpu(cpu) {
		free_page((unsigned long)per_cpu(trace_buffered_event, cpu));
		per_cpu(trace_buffered_event, cpu) = NULL;
	}
	/*
	 * Make sure trace_buffered_event is NULL before clearing
	 * trace_buffered_event_cnt.
	 */
	smp_wmb();

	preempt_disable();
	/* Do the work on each cpu */
	smp_call_function_many(tracing_buffer_mask,
			       enable_trace_buffered_event, NULL, 1);
	preempt_enable();
}