trace_function(struct trace_array *tr,
	       unsigned long ip, unsigned long parent_ip, unsigned long flags,
	       int pc)
{
	struct trace_event_call *call = &event_function;
	struct ring_buffer *buffer = tr->trace_buffer.buffer;
	struct ring_buffer_event *event;
	struct ftrace_entry *entry;

	event = __trace_buffer_lock_reserve(buffer, TRACE_FN, sizeof(*entry),
					    flags, pc);
	if (!event)
		return;
	entry	= ring_buffer_event_data(event);
	entry->ip			= ip;
	entry->parent_ip		= parent_ip;

	if (!call_filter_check_discard(call, entry, buffer, event)) {
		if (static_branch_unlikely(&ftrace_exports_enabled))
			ftrace_exports(event);
		__buffer_unlock_commit(buffer, event);
	}
}