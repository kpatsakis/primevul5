static void v4l_print_tuner(const void *arg, bool write_only)
{
	const struct v4l2_tuner *p = arg;

	if (write_only)
		pr_cont("index=%u, audmode=%u\n", p->index, p->audmode);
	else
		pr_cont("index=%u, name=%.*s, type=%u, capability=0x%x, rangelow=%u, rangehigh=%u, signal=%u, afc=%d, rxsubchans=0x%x, audmode=%u\n",
			p->index, (int)sizeof(p->name), p->name, p->type,
			p->capability, p->rangelow,
			p->rangehigh, p->signal, p->afc,
			p->rxsubchans, p->audmode);
}