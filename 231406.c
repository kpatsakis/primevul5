static void init_trace_flags_index(struct trace_array *tr)
{
	int i;

	/* Used by the trace options files */
	for (i = 0; i < TRACE_FLAGS_MAX_SIZE; i++)
		tr->trace_flags_index[i] = i;
}