static void v4l_print_standard(const void *arg, bool write_only)
{
	const struct v4l2_standard *p = arg;

	pr_cont("index=%u, id=0x%Lx, name=%.*s, fps=%u/%u, framelines=%u\n",
		p->index,
		(unsigned long long)p->id, (int)sizeof(p->name), p->name,
		p->frameperiod.numerator,
		p->frameperiod.denominator,
		p->framelines);
}