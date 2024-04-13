static void trace_consume(struct trace_iterator *iter)
{
	ring_buffer_consume(iter->trace_buffer->buffer, iter->cpu, &iter->ts,
			    &iter->lost_events);
}