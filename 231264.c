void trace_dump_stack(int skip)
{
	unsigned long flags;

	if (tracing_disabled || tracing_selftest_running)
		return;

	local_save_flags(flags);

	/*
	 * Skip 3 more, seems to get us at the caller of
	 * this function.
	 */
	skip += 3;
	__ftrace_trace_stack(global_trace.trace_buffer.buffer,
			     flags, skip, preempt_count(), NULL);
}