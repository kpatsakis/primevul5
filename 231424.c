ftrace_count_snapshot(unsigned long ip, unsigned long parent_ip,
		      struct trace_array *tr, struct ftrace_probe_ops *ops,
		      void *data)
{
	struct ftrace_func_mapper *mapper = data;
	long *count = NULL;

	if (mapper)
		count = (long *)ftrace_func_mapper_find_ip(mapper, ip);

	if (count) {

		if (*count <= 0)
			return;

		(*count)--;
	}

	tracing_snapshot_instance(tr);
}