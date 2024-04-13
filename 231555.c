static enum print_line_t print_hex_fmt(struct trace_iterator *iter)
{
	struct trace_array *tr = iter->tr;
	struct trace_seq *s = &iter->seq;
	unsigned char newline = '\n';
	struct trace_entry *entry;
	struct trace_event *event;

	entry = iter->ent;

	if (tr->trace_flags & TRACE_ITER_CONTEXT_INFO) {
		SEQ_PUT_HEX_FIELD(s, entry->pid);
		SEQ_PUT_HEX_FIELD(s, iter->cpu);
		SEQ_PUT_HEX_FIELD(s, iter->ts);
		if (trace_seq_has_overflowed(s))
			return TRACE_TYPE_PARTIAL_LINE;
	}

	event = ftrace_find_event(entry->type);
	if (event) {
		enum print_line_t ret = event->funcs->hex(iter, 0, event);
		if (ret != TRACE_TYPE_HANDLED)
			return ret;
	}

	SEQ_PUT_FIELD(s, newline);

	return trace_handle_return(s);
}