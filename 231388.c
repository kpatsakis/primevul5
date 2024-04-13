trace_ok_for_array(struct tracer *t, struct trace_array *tr)
{
	return (tr->flags & TRACE_ARRAY_FL_GLOBAL) || t->allow_instances;
}