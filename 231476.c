void tracing_reset_all_online_cpus(void)
{
	struct trace_array *tr;

	list_for_each_entry(tr, &ftrace_trace_arrays, list) {
		if (!tr->clear_trace)
			continue;
		tr->clear_trace = false;
		tracing_reset_online_cpus(&tr->trace_buffer);
#ifdef CONFIG_TRACER_MAX_TRACE
		tracing_reset_online_cpus(&tr->max_buffer);
#endif
	}
}