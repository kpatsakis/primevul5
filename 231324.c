void tracing_snapshot(void)
{
	struct trace_array *tr = &global_trace;

	tracing_snapshot_instance(tr);
}