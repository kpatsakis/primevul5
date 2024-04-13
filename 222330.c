static void input_seq_print_bitmap(struct seq_file *seq, const char *name,
				   unsigned long *bitmap, int max)
{
	int i;
	bool skip_empty = true;
	char buf[18];

	seq_printf(seq, "B: %s=", name);

	for (i = BITS_TO_LONGS(max) - 1; i >= 0; i--) {
		if (input_bits_to_string(buf, sizeof(buf),
					 bitmap[i], skip_empty)) {
			skip_empty = false;
			seq_printf(seq, "%s%s", buf, i > 0 ? " " : "");
		}
	}

	/*
	 * If no output was produced print a single 0.
	 */
	if (skip_empty)
		seq_putc(seq, '0');

	seq_putc(seq, '\n');
}