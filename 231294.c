int tracing_update_buffers(void)
{
	int ret = 0;

	mutex_lock(&trace_types_lock);
	if (!ring_buffer_expanded)
		ret = __tracing_resize_ring_buffer(&global_trace, trace_buf_size,
						RING_BUFFER_ALL_CPUS);
	mutex_unlock(&trace_types_lock);

	return ret;
}