static void create_trace_options_dir(struct trace_array *tr)
{
	struct dentry *t_options;
	bool top_level = tr == &global_trace;
	int i;

	t_options = trace_options_init_dentry(tr);
	if (!t_options)
		return;

	for (i = 0; trace_options[i]; i++) {
		if (top_level ||
		    !((1 << i) & TOP_LEVEL_TRACE_FLAGS))
			create_trace_option_core_file(tr, trace_options[i], i);
	}
}