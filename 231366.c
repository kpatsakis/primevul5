__buffer_unlock_commit(struct ring_buffer *buffer, struct ring_buffer_event *event)
{
	__this_cpu_write(trace_taskinfo_save, true);

	/* If this is the temp buffer, we need to commit fully */
	if (this_cpu_read(trace_buffered_event) == event) {
		/* Length is in event->array[0] */
		ring_buffer_write(buffer, event->array[0], &event->array[1]);
		/* Release the temp buffer */
		this_cpu_dec(trace_buffered_event_cnt);
	} else
		ring_buffer_unlock_commit(buffer, event);
}