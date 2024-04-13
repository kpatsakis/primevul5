get_total_entries(struct trace_buffer *buf,
		  unsigned long *total, unsigned long *entries)
{
	unsigned long count;
	int cpu;

	*total = 0;
	*entries = 0;

	for_each_tracing_cpu(cpu) {
		count = ring_buffer_entries_cpu(buf->buffer, cpu);
		/*
		 * If this buffer has skipped entries, then we hold all
		 * entries for the trace and we need to ignore the
		 * ones before the time stamp.
		 */
		if (per_cpu_ptr(buf->data, cpu)->skipped_entries) {
			count -= per_cpu_ptr(buf->data, cpu)->skipped_entries;
			/* total is the same as the entries */
			*total += count;
		} else
			*total += count +
				ring_buffer_overrun_cpu(buf->buffer, cpu);
		*entries += count;
	}
}