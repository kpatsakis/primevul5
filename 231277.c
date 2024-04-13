rm_trace_export(struct trace_export **list, struct trace_export *export)
{
	struct trace_export **p;

	for (p = list; *p != NULL; p = &(*p)->next)
		if (*p == export)
			break;

	if (*p != export)
		return -1;

	rcu_assign_pointer(*p, (*p)->next);

	return 0;
}