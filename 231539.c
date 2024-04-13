__trace_array_vprintk(struct ring_buffer *buffer,
		      unsigned long ip, const char *fmt, va_list args)
{
	struct trace_event_call *call = &event_print;
	struct ring_buffer_event *event;
	int len = 0, size, pc;
	struct print_entry *entry;
	unsigned long flags;
	char *tbuffer;

	if (tracing_disabled || tracing_selftest_running)
		return 0;

	/* Don't pollute graph traces with trace_vprintk internals */
	pause_graph_tracing();

	pc = preempt_count();
	preempt_disable_notrace();


	tbuffer = get_trace_buf();
	if (!tbuffer) {
		len = 0;
		goto out_nobuffer;
	}

	len = vscnprintf(tbuffer, TRACE_BUF_SIZE, fmt, args);

	local_save_flags(flags);
	size = sizeof(*entry) + len + 1;
	event = __trace_buffer_lock_reserve(buffer, TRACE_PRINT, size,
					    flags, pc);
	if (!event)
		goto out;
	entry = ring_buffer_event_data(event);
	entry->ip = ip;

	memcpy(&entry->buf, tbuffer, len + 1);
	if (!call_filter_check_discard(call, entry, buffer, event)) {
		__buffer_unlock_commit(buffer, event);
		ftrace_trace_stack(&global_trace, buffer, flags, 6, pc, NULL);
	}

out:
	put_trace_buf();

out_nobuffer:
	preempt_enable_notrace();
	unpause_graph_tracing();

	return len;
}