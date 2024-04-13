ftrace_snapshot(unsigned long ip, unsigned long parent_ip,
		struct trace_array *tr, struct ftrace_probe_ops *ops,
		void *data)
{
	tracing_snapshot_instance(tr);
}