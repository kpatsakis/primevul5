void trace_latency_header(struct seq_file *m)
{
	struct trace_iterator *iter = m->private;
	struct trace_array *tr = iter->tr;

	/* print nothing if the buffers are empty */
	if (trace_empty(iter))
		return;

	if (iter->iter_flags & TRACE_FILE_LAT_FMT)
		print_trace_header(m, iter);

	if (!(tr->trace_flags & TRACE_ITER_VERBOSE))
		print_lat_help_header(m);
}