static inline void ftrace_trace_stack(struct trace_array *tr,
				      struct ring_buffer *buffer,
				      unsigned long flags,
				      int skip, int pc, struct pt_regs *regs)
{
	if (!(tr->trace_flags & TRACE_ITER_STACKTRACE))
		return;

	__ftrace_trace_stack(buffer, flags, skip, pc, regs);
}