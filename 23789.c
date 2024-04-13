char *fmtalloc(const char *format, ...)
{
	struct strbuf sb = STRBUF_INIT;
	va_list args;

	va_start(args, format);
	strbuf_vaddf(&sb, format, args);
	va_end(args);

	return strbuf_detach(&sb, NULL);
}