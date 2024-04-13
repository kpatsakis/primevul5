int trace_array_get(struct trace_array *this_tr)
{
	struct trace_array *tr;
	int ret = -ENODEV;

	mutex_lock(&trace_types_lock);
	list_for_each_entry(tr, &ftrace_trace_arrays, list) {
		if (tr == this_tr) {
			tr->ref++;
			ret = 0;
			break;
		}
	}
	mutex_unlock(&trace_types_lock);

	return ret;
}