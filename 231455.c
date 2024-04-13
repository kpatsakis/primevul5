trace_buffer_unlock_commit_nostack(struct ring_buffer *buffer,
				   struct ring_buffer_event *event)
{
	__buffer_unlock_commit(buffer, event);
}