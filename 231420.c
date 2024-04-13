trace_buffer_lock_reserve(struct ring_buffer *buffer,
			  int type,
			  unsigned long len,
			  unsigned long flags, int pc)
{
	return __trace_buffer_lock_reserve(buffer, type, len, flags, pc);
}