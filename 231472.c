tracing_set_trace_read(struct file *filp, char __user *ubuf,
		       size_t cnt, loff_t *ppos)
{
	struct trace_array *tr = filp->private_data;
	char buf[MAX_TRACER_SIZE+2];
	int r;

	mutex_lock(&trace_types_lock);
	r = sprintf(buf, "%s\n", tr->current_trace->name);
	mutex_unlock(&trace_types_lock);

	return simple_read_from_buffer(ubuf, cnt, ppos, buf, r);
}