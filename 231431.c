static int alloc_percpu_trace_buffer(void)
{
	struct trace_buffer_struct *buffers;

	buffers = alloc_percpu(struct trace_buffer_struct);
	if (WARN(!buffers, "Could not allocate percpu trace_printk buffer"))
		return -ENOMEM;

	trace_percpu_buffer = buffers;
	return 0;
}