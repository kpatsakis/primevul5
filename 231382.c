allocate_trace_buffer(struct trace_array *tr, struct trace_buffer *buf, int size)
{
	enum ring_buffer_flags rb_flags;

	rb_flags = tr->trace_flags & TRACE_ITER_OVERWRITE ? RB_FL_OVERWRITE : 0;

	buf->tr = tr;

	buf->buffer = ring_buffer_alloc(size, rb_flags);
	if (!buf->buffer)
		return -ENOMEM;

	buf->data = alloc_percpu(struct trace_array_cpu);
	if (!buf->data) {
		ring_buffer_free(buf->buffer);
		buf->buffer = NULL;
		return -ENOMEM;
	}

	/* Allocate the first page for all buffers */
	set_buffer_entries(&tr->trace_buffer,
			   ring_buffer_size(tr->trace_buffer.buffer, 0));

	return 0;
}