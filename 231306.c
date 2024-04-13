static struct dentry *trace_options_init_dentry(struct trace_array *tr)
{
	struct dentry *d_tracer;

	if (tr->options)
		return tr->options;

	d_tracer = tracing_get_dentry(tr);
	if (IS_ERR(d_tracer))
		return NULL;

	tr->options = tracefs_create_dir("options", d_tracer);
	if (!tr->options) {
		pr_warn("Could not create tracefs directory 'options'\n");
		return NULL;
	}

	return tr->options;
}