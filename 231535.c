trace_event_buffer_lock_reserve(struct ring_buffer **current_rb,
			  struct trace_event_file *trace_file,
			  int type, unsigned long len,
			  unsigned long flags, int pc)
{
	struct ring_buffer_event *entry;
	int val;

	*current_rb = trace_file->tr->trace_buffer.buffer;

	if ((trace_file->flags &
	     (EVENT_FILE_FL_SOFT_DISABLED | EVENT_FILE_FL_FILTERED)) &&
	    (entry = this_cpu_read(trace_buffered_event))) {
		/* Try to use the per cpu buffer first */
		val = this_cpu_inc_return(trace_buffered_event_cnt);
		if (val == 1) {
			trace_event_setup(entry, type, flags, pc);
			entry->array[0] = len;
			return entry;
		}
		this_cpu_dec(trace_buffered_event_cnt);
	}

	entry = __trace_buffer_lock_reserve(*current_rb,
					    type, len, flags, pc);
	/*
	 * If tracing is off, but we have triggers enabled
	 * we still need to look at the event data. Use the temp_buffer
	 * to store the trace event for the tigger to use. It's recusive
	 * safe and will not be recorded anywhere.
	 */
	if (!entry && trace_file->flags & EVENT_FILE_FL_TRIGGER_COND) {
		*current_rb = temp_buffer;
		entry = __trace_buffer_lock_reserve(*current_rb,
						    type, len, flags, pc);
	}
	return entry;
}