void cgit_print_error(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	cgit_vprint_error(fmt, ap);
	va_end(ap);
}