char *fmt(const char *format, ...)
{
	static char buf[8][1024];
	static int bufidx;
	int len;
	va_list args;

	bufidx++;
	bufidx &= 7;

	va_start(args, format);
	len = vsnprintf(buf[bufidx], sizeof(buf[bufidx]), format, args);
	va_end(args);
	if (len>sizeof(buf[bufidx])) {
		fprintf(stderr, "[html.c] string truncated: %s\n", format);
		exit(1);
	}
	return buf[bufidx];
}