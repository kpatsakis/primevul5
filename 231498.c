int register_ftrace_export(struct trace_export *export)
{
	if (WARN_ON_ONCE(!export->write))
		return -1;

	mutex_lock(&ftrace_export_lock);

	add_ftrace_export(&ftrace_exports_list, export);

	mutex_unlock(&ftrace_export_lock);

	return 0;
}