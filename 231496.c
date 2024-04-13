static char *get_trace_buf(void)
{
	struct trace_buffer_struct *buffer = this_cpu_ptr(trace_percpu_buffer);

	if (!buffer || buffer->nesting >= 4)
		return NULL;

	buffer->nesting++;

	/* Interrupts must see nesting incremented before we use the buffer */
	barrier();
	return &buffer->buffer[buffer->nesting][0];
}