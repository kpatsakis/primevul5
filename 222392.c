static int input_print_modalias_bits(char *buf, int size,
				     char name, unsigned long *bm,
				     unsigned int min_bit, unsigned int max_bit)
{
	int len = 0, i;

	len += snprintf(buf, max(size, 0), "%c", name);
	for (i = min_bit; i < max_bit; i++)
		if (bm[BIT_WORD(i)] & BIT_MASK(i))
			len += snprintf(buf + len, max(size - len, 0), "%X,", i);
	return len;
}