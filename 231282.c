void trace_array_put(struct trace_array *this_tr)
{
	mutex_lock(&trace_types_lock);
	__trace_array_put(this_tr);
	mutex_unlock(&trace_types_lock);
}