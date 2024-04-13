int trace_array_printk_buf(struct ring_buffer *buffer,
			   unsigned long ip, const char *fmt, ...)
{
	int ret;
	va_list ap;

	if (!(global_trace.trace_flags & TRACE_ITER_PRINTK))
		return 0;

	va_start(ap, fmt);
	ret = __trace_array_vprintk(buffer, ip, fmt, ap);
	va_end(ap);
	return ret;
}