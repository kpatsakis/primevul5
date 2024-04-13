static void free_trace_buffer(struct trace_buffer *buf)
{
	if (buf->buffer) {
		ring_buffer_free(buf->buffer);
		buf->buffer = NULL;
		free_percpu(buf->data);
		buf->data = NULL;
	}
}