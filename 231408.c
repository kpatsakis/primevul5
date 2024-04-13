trace_printk_seq(struct trace_seq *s)
{
	/* Probably should print a warning here. */
	if (s->seq.len >= TRACE_MAX_PRINT)
		s->seq.len = TRACE_MAX_PRINT;

	/*
	 * More paranoid code. Although the buffer size is set to
	 * PAGE_SIZE, and TRACE_MAX_PRINT is 1000, this is just
	 * an extra layer of protection.
	 */
	if (WARN_ON_ONCE(s->seq.len >= s->seq.size))
		s->seq.len = s->seq.size - 1;

	/* should be zero ended, but we are paranoid. */
	s->buffer[s->seq.len] = 0;

	printk(KERN_TRACE "%s", s->buffer);

	trace_seq_init(s);
}