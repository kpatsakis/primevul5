void html_attrf(const char *fmt, ...)
{
	va_list ap;
	struct strbuf sb = STRBUF_INIT;

	va_start(ap, fmt);
	strbuf_vaddf(&sb, fmt, ap);
	va_end(ap);

	html_attr(sb.buf);
	strbuf_release(&sb);
}