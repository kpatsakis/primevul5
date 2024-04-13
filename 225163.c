static void v4l_print_hw_freq_seek(const void *arg, bool write_only)
{
	const struct v4l2_hw_freq_seek *p = arg;

	pr_cont("tuner=%u, type=%u, seek_upward=%u, wrap_around=%u, spacing=%u, rangelow=%u, rangehigh=%u\n",
		p->tuner, p->type, p->seek_upward, p->wrap_around, p->spacing,
		p->rangelow, p->rangehigh);
}