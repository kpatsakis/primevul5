int zend_spprintf(char **message, int max_len, char *format, ...) /* {{{ */
{
	va_list arg;
	int len;

	va_start(arg, format);
	len = zend_vspprintf(message, max_len, format, arg);
	va_end(arg);
	return len;
}