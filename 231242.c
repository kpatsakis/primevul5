add_trace_export(struct trace_export **list, struct trace_export *export)
{
	rcu_assign_pointer(export->next, *list);
	/*
	 * We are entering export into the list but another
	 * CPU might be walking that list. We need to make sure
	 * the export->next pointer is valid before another CPU sees
	 * the export pointer included into the list.
	 */
	rcu_assign_pointer(*list, export);
}