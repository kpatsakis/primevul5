tracing_total_entries_read(struct file *filp, char __user *ubuf,
				size_t cnt, loff_t *ppos)
{
	struct trace_array *tr = filp->private_data;
	char buf[64];
	int r, cpu;
	unsigned long size = 0, expanded_size = 0;

	mutex_lock(&trace_types_lock);
	for_each_tracing_cpu(cpu) {
		size += per_cpu_ptr(tr->trace_buffer.data, cpu)->entries >> 10;
		if (!ring_buffer_expanded)
			expanded_size += trace_buf_size >> 10;
	}
	if (ring_buffer_expanded)
		r = sprintf(buf, "%lu\n", size);
	else
		r = sprintf(buf, "%lu (expanded: %lu)\n", size, expanded_size);
	mutex_unlock(&trace_types_lock);

	return simple_read_from_buffer(ubuf, cnt, ppos, buf, r);
}