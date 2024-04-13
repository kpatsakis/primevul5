static void v4l_print_audio(const void *arg, bool write_only)
{
	const struct v4l2_audio *p = arg;

	if (write_only)
		pr_cont("index=%u, mode=0x%x\n", p->index, p->mode);
	else
		pr_cont("index=%u, name=%.*s, capability=0x%x, mode=0x%x\n",
			p->index, (int)sizeof(p->name), p->name,
			p->capability, p->mode);
}