init_tracer_tracefs(struct trace_array *tr, struct dentry *d_tracer)
{
	int cpu;

	trace_create_file("available_tracers", 0444, d_tracer,
			tr, &show_traces_fops);

	trace_create_file("current_tracer", 0644, d_tracer,
			tr, &set_tracer_fops);

	trace_create_file("tracing_cpumask", 0644, d_tracer,
			  tr, &tracing_cpumask_fops);

	trace_create_file("trace_options", 0644, d_tracer,
			  tr, &tracing_iter_fops);

	trace_create_file("trace", 0644, d_tracer,
			  tr, &tracing_fops);

	trace_create_file("trace_pipe", 0444, d_tracer,
			  tr, &tracing_pipe_fops);

	trace_create_file("buffer_size_kb", 0644, d_tracer,
			  tr, &tracing_entries_fops);

	trace_create_file("buffer_total_size_kb", 0444, d_tracer,
			  tr, &tracing_total_entries_fops);

	trace_create_file("free_buffer", 0200, d_tracer,
			  tr, &tracing_free_buffer_fops);

	trace_create_file("trace_marker", 0220, d_tracer,
			  tr, &tracing_mark_fops);

	trace_create_file("trace_marker_raw", 0220, d_tracer,
			  tr, &tracing_mark_raw_fops);

	trace_create_file("trace_clock", 0644, d_tracer, tr,
			  &trace_clock_fops);

	trace_create_file("tracing_on", 0644, d_tracer,
			  tr, &rb_simple_fops);

	create_trace_options_dir(tr);

#if defined(CONFIG_TRACER_MAX_TRACE) || defined(CONFIG_HWLAT_TRACER)
	trace_create_file("tracing_max_latency", 0644, d_tracer,
			&tr->max_latency, &tracing_max_lat_fops);
#endif

	if (ftrace_create_function_files(tr, d_tracer))
		WARN(1, "Could not allocate function filter files");

#ifdef CONFIG_TRACER_SNAPSHOT
	trace_create_file("snapshot", 0644, d_tracer,
			  tr, &snapshot_fops);
#endif

	for_each_tracing_cpu(cpu)
		tracing_init_tracefs_percpu(tr, cpu);

	ftrace_init_tracefs(tr, d_tracer);
}