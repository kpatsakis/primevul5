int unregister_ftrace_export(struct trace_export *export)
{
	int ret;

	mutex_lock(&ftrace_export_lock);

	ret = rm_ftrace_export(&ftrace_exports_list, export);

	mutex_unlock(&ftrace_export_lock);

	return ret;
}