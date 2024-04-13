void cgit_vprint_error(const char *fmt, va_list ap)
{
	va_list cp;
	html("<div class='error'>");
	va_copy(cp, ap);
	html_vtxtf(fmt, cp);
	va_end(cp);
	html("</div>\n");
}