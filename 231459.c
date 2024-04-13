tracing_entries_read(struct file *filp, char __user *ubuf,
		     size_t cnt, loff_t *ppos)
{
	struct inode *inode = file_inode(filp);
	struct trace_array *tr = inode->i_private;
	int cpu = tracing_get_cpu(inode);
	char buf[64];
	int r = 0;
	ssize_t ret;

	mutex_lock(&trace_types_lock);

	if (cpu == RING_BUFFER_ALL_CPUS) {
		int cpu, buf_size_same;
		unsigned long size;

		size = 0;
		buf_size_same = 1;
		/* check if all cpu sizes are same */
		for_each_tracing_cpu(cpu) {
			/* fill in the size from first enabled cpu */
			if (size == 0)
				size = per_cpu_ptr(tr->trace_buffer.data, cpu)->entries;
			if (size != per_cpu_ptr(tr->trace_buffer.data, cpu)->entries) {
				buf_size_same = 0;
				break;
			}
		}

		if (buf_size_same) {
			if (!ring_buffer_expanded)
				r = sprintf(buf, "%lu (expanded: %lu)\n",
					    size >> 10,
					    trace_buf_size >> 10);
			else
				r = sprintf(buf, "%lu\n", size >> 10);
		} else
			r = sprintf(buf, "X\n");
	} else
		r = sprintf(buf, "%lu\n", per_cpu_ptr(tr->trace_buffer.data, cpu)->entries >> 10);

	mutex_unlock(&trace_types_lock);

	ret = simple_read_from_buffer(ubuf, cnt, ppos, buf, r);
	return ret;
}