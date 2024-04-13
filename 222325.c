static int input_bits_to_string(char *buf, int buf_size,
				unsigned long bits, bool skip_empty)
{
	return bits || !skip_empty ?
		snprintf(buf, buf_size, "%lx", bits) : 0;
}