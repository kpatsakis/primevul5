void htmlf(const char *format, ...)
{
	va_list args;
	struct strbuf buf = STRBUF_INIT;

	va_start(args, format);
	strbuf_vaddf(&buf, format, args);
	va_end(args);
	html(buf.buf);
	strbuf_release(&buf);
}