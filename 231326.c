enum print_line_t trace_handle_return(struct trace_seq *s)
{
	return trace_seq_has_overflowed(s) ?
		TRACE_TYPE_PARTIAL_LINE : TRACE_TYPE_HANDLED;
}