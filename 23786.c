void html_txtf(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	html_vtxtf(format, args);
	va_end(args);
}