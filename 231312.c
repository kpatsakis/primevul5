int trace_array_vprintk(struct trace_array *tr,
			unsigned long ip, const char *fmt, va_list args)
{
	return __trace_array_vprintk(tr->trace_buffer.buffer, ip, fmt, args);
}