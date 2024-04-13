create_trace_option_core_file(struct trace_array *tr,
			      const char *option, long index)
{
	struct dentry *t_options;

	t_options = trace_options_init_dentry(tr);
	if (!t_options)
		return NULL;

	return trace_create_file(option, 0644, t_options,
				 (void *)&tr->trace_flags_index[index],
				 &trace_options_core_fops);
}