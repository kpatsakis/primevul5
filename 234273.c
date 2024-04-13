void htmlf(const char *format, ...)
{
	static char buf[65536];
	va_list args;

	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	va_end(args);
	html(buf);
}