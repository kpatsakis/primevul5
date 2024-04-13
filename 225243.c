static void v4l_print_freq_band(const void *arg, bool write_only)
{
	const struct v4l2_frequency_band *p = arg;

	pr_cont("tuner=%u, type=%u, index=%u, capability=0x%x, rangelow=%u, rangehigh=%u, modulation=0x%x\n",
			p->tuner, p->type, p->index,
			p->capability, p->rangelow,
			p->rangehigh, p->modulation);
}