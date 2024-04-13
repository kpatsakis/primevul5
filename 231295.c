static void __update_tracer_options(struct trace_array *tr)
{
	struct tracer *t;

	for (t = trace_types; t; t = t->next)
		add_tracer_options(tr, t);
}