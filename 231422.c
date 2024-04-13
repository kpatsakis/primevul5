rm_ftrace_export(struct trace_export **list, struct trace_export *export)
{
	int ret;

	ret = rm_trace_export(list, export);
	if (*list == NULL)
		ftrace_exports_disable();

	return ret;
}