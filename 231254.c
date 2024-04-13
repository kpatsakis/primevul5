static enum print_line_t print_raw_fmt(struct trace_iterator *iter)
{
	struct trace_array *tr = iter->tr;
	struct trace_seq *s = &iter->seq;
	struct trace_entry *entry;
	struct trace_event *event;

	entry = iter->ent;

	if (tr->trace_flags & TRACE_ITER_CONTEXT_INFO)
		trace_seq_printf(s, "%d %d %llu ",
				 entry->pid, iter->cpu, iter->ts);

	if (trace_seq_has_overflowed(s))
		return TRACE_TYPE_PARTIAL_LINE;

	event = ftrace_find_event(entry->type);
	if (event)
		return event->funcs->raw(iter, 0, event);

	trace_seq_printf(s, "%d ?\n", entry->type);

	return trace_handle_return(s);
}