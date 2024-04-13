static void put_trace_buf(void)
{
	/* Don't let the decrement of nesting leak before this */
	barrier();
	this_cpu_dec(trace_percpu_buffer->nesting);
}