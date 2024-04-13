get_tracer_for_array(struct trace_array *tr, struct tracer *t)
{
	while (t && !trace_ok_for_array(t, tr))
		t = t->next;

	return t;
}