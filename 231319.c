int tracing_alloc_snapshot(void)
{
	struct trace_array *tr = &global_trace;
	int ret;

	ret = alloc_snapshot(tr);
	WARN_ON(ret < 0);

	return ret;
}