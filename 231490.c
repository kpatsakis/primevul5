add_ftrace_export(struct trace_export **list, struct trace_export *export)
{
	if (*list == NULL)
		ftrace_exports_enable();

	add_trace_export(list, export);
}