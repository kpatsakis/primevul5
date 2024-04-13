ftrace_snapshot_free(struct ftrace_probe_ops *ops, struct trace_array *tr,
		     unsigned long ip, void *data)
{
	struct ftrace_func_mapper *mapper = data;

	if (!ip) {
		if (!mapper)
			return;
		free_ftrace_func_mapper(mapper, NULL);
		return;
	}

	ftrace_func_mapper_remove_ip(mapper, ip);
}