static void __ftrace_trace_stack(struct ring_buffer *buffer,
				 unsigned long flags,
				 int skip, int pc, struct pt_regs *regs)
{
	struct trace_event_call *call = &event_kernel_stack;
	struct ring_buffer_event *event;
	struct stack_entry *entry;
	struct stack_trace trace;
	int use_stack;
	int size = FTRACE_STACK_ENTRIES;

	trace.nr_entries	= 0;
	trace.skip		= skip;

	/*
	 * Add two, for this function and the call to save_stack_trace()
	 * If regs is set, then these functions will not be in the way.
	 */
	if (!regs)
		trace.skip += 2;

	/*
	 * Since events can happen in NMIs there's no safe way to
	 * use the per cpu ftrace_stacks. We reserve it and if an interrupt
	 * or NMI comes in, it will just have to use the default
	 * FTRACE_STACK_SIZE.
	 */
	preempt_disable_notrace();

	use_stack = __this_cpu_inc_return(ftrace_stack_reserve);
	/*
	 * We don't need any atomic variables, just a barrier.
	 * If an interrupt comes in, we don't care, because it would
	 * have exited and put the counter back to what we want.
	 * We just need a barrier to keep gcc from moving things
	 * around.
	 */
	barrier();
	if (use_stack == 1) {
		trace.entries		= this_cpu_ptr(ftrace_stack.calls);
		trace.max_entries	= FTRACE_STACK_MAX_ENTRIES;

		if (regs)
			save_stack_trace_regs(regs, &trace);
		else
			save_stack_trace(&trace);

		if (trace.nr_entries > size)
			size = trace.nr_entries;
	} else
		/* From now on, use_stack is a boolean */
		use_stack = 0;

	size *= sizeof(unsigned long);

	event = __trace_buffer_lock_reserve(buffer, TRACE_STACK,
					    sizeof(*entry) + size, flags, pc);
	if (!event)
		goto out;
	entry = ring_buffer_event_data(event);

	memset(&entry->caller, 0, size);

	if (use_stack)
		memcpy(&entry->caller, trace.entries,
		       trace.nr_entries * sizeof(unsigned long));
	else {
		trace.max_entries	= FTRACE_STACK_ENTRIES;
		trace.entries		= entry->caller;
		if (regs)
			save_stack_trace_regs(regs, &trace);
		else
			save_stack_trace(&trace);
	}

	entry->size = trace.nr_entries;

	if (!call_filter_check_discard(call, entry, buffer, event))
		__buffer_unlock_commit(buffer, event);

 out:
	/* Again, don't let gcc optimize things here */
	barrier();
	__this_cpu_dec(ftrace_stack_reserve);
	preempt_enable_notrace();

}