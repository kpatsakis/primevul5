static void tracing_set_nop(struct trace_array *tr)
{
	if (tr->current_trace == &nop_trace)
		return;
	
	tr->current_trace->enabled--;

	if (tr->current_trace->reset)
		tr->current_trace->reset(tr);

	tr->current_trace = &nop_trace;
}