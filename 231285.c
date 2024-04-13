int trace_vprintk(unsigned long ip, const char *fmt, va_list args)
{
	return trace_array_vprintk(&global_trace, ip, fmt, args);
}