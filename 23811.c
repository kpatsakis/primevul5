void html_raw(const char *data, size_t size)
{
	if (write(STDOUT_FILENO, data, size) != size)
		die_errno("write error on html output");
}