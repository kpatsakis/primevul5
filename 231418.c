int tracing_is_enabled(void)
{
	/*
	 * For quick access (irqsoff uses this in fast path), just
	 * return the mirror variable of the state of the ring buffer.
	 * It's a little racy, but we don't really care.
	 */
	smp_rmb();
	return !global_trace.buffer_disabled;
}