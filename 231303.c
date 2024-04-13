static void set_buffer_entries(struct trace_buffer *buf, unsigned long val)
{
	int cpu;

	for_each_tracing_cpu(cpu)
		per_cpu_ptr(buf->data, cpu)->entries = val;
}