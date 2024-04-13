static void add_tracer_options(struct trace_array *tr, struct tracer *t)
{
	/* Only enable if the directory has been created already. */
	if (!tr->dir)
		return;

	create_trace_option_files(tr, t);
}