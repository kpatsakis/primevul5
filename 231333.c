static struct dentry *tracing_dentry_percpu(struct trace_array *tr, int cpu)
{
	struct dentry *d_tracer;

	if (tr->percpu_dir)
		return tr->percpu_dir;

	d_tracer = tracing_get_dentry(tr);
	if (IS_ERR(d_tracer))
		return NULL;

	tr->percpu_dir = tracefs_create_dir("per_cpu", d_tracer);

	WARN_ONCE(!tr->percpu_dir,
		  "Could not create tracefs directory 'per_cpu/%d'\n", cpu);

	return tr->percpu_dir;
}