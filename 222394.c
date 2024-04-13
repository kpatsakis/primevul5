static int input_print_bitmap(char *buf, int buf_size, unsigned long *bitmap,
			      int max, int add_cr)
{
	int i;
	int len = 0;
	bool skip_empty = true;

	for (i = BITS_TO_LONGS(max) - 1; i >= 0; i--) {
		len += input_bits_to_string(buf + len, max(buf_size - len, 0),
					    bitmap[i], skip_empty);
		if (len) {
			skip_empty = false;
			if (i > 0)
				len += snprintf(buf + len, max(buf_size - len, 0), " ");
		}
	}

	/*
	 * If no output was produced print a single 0.
	 */
	if (len == 0)
		len = snprintf(buf, buf_size, "%d", 0);

	if (add_cr)
		len += snprintf(buf + len, max(buf_size - len, 0), "\n");

	return len;
}