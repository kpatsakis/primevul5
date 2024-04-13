void html_vtxtf(const char *format, va_list ap)
{
	va_list cp;
	struct strbuf buf = STRBUF_INIT;

	va_copy(cp, ap);
	strbuf_vaddf(&buf, format, cp);
	va_end(cp);
	html_txt(buf.buf);
	strbuf_release(&buf);
}