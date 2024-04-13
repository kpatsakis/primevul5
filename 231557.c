ftrace_trace_userstack(struct ring_buffer *buffer, unsigned long flags, int pc)
{
	struct trace_event_call *call = &event_user_stack;
	struct ring_buffer_event *event;
	struct userstack_entry *entry;
	struct stack_trace trace;

	if (!(global_trace.trace_flags & TRACE_ITER_USERSTACKTRACE))
		return;

	/*
	 * NMIs can not handle page faults, even with fix ups.
	 * The save user stack can (and often does) fault.
	 */
	if (unlikely(in_nmi()))
		return;

	/*
	 * prevent recursion, since the user stack tracing may
	 * trigger other kernel events.
	 */
	preempt_disable();
	if (__this_cpu_read(user_stack_count))
		goto out;

	__this_cpu_inc(user_stack_count);

	event = __trace_buffer_lock_reserve(buffer, TRACE_USER_STACK,
					    sizeof(*entry), flags, pc);
	if (!event)
		goto out_drop_count;
	entry	= ring_buffer_event_data(event);

	entry->tgid		= current->tgid;
	memset(&entry->caller, 0, sizeof(entry->caller));

	trace.nr_entries	= 0;
	trace.max_entries	= FTRACE_STACK_ENTRIES;
	trace.skip		= 0;
	trace.entries		= entry->caller;

	save_stack_trace_user(&trace);
	if (!call_filter_check_discard(call, entry, buffer, event))
		__buffer_unlock_commit(buffer, event);

 out_drop_count:
	__this_cpu_dec(user_stack_count);
 out:
	preempt_enable();
}