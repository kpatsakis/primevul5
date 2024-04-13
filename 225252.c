static void v4l_print_modulator(const void *arg, bool write_only)
{
	const struct v4l2_modulator *p = arg;

	if (write_only)
		pr_cont("index=%u, txsubchans=0x%x\n", p->index, p->txsubchans);
	else
		pr_cont("index=%u, name=%.*s, capability=0x%x, rangelow=%u, rangehigh=%u, txsubchans=0x%x\n",
			p->index, (int)sizeof(p->name), p->name, p->capability,
			p->rangelow, p->rangehigh, p->txsubchans);
}