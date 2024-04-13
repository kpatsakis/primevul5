static int __init set_buf_size(char *str)
{
	unsigned long buf_size;

	if (!str)
		return 0;
	buf_size = memparse(str, &str);
	/* nr_entries can not be zero */
	if (buf_size == 0)
		return 0;
	trace_buf_size = buf_size;
	return 1;
}