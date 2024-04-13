static void update_tracer_options(struct trace_array *tr)
{
	mutex_lock(&trace_types_lock);
	__update_tracer_options(tr);
	mutex_unlock(&trace_types_lock);
}