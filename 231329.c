void trace_buffer_unlock_commit_regs(struct trace_array *tr,
				     struct ring_buffer *buffer,
				     struct ring_buffer_event *event,
				     unsigned long flags, int pc,
				     struct pt_regs *regs)
{
	__buffer_unlock_commit(buffer, event);

	/*
	 * If regs is not set, then skip the following callers:
	 *   trace_buffer_unlock_commit_regs
	 *   event_trigger_unlock_commit
	 *   trace_event_buffer_commit
	 *   trace_event_raw_event_sched_switch
	 * Note, we can still get here via blktrace, wakeup tracer
	 * and mmiotrace, but that's ok if they lose a function or
	 * two. They are that meaningful.
	 */
	ftrace_trace_stack(tr, buffer, flags, regs ? 0 : 4, pc, regs);
	ftrace_trace_userstack(buffer, flags, pc);
}