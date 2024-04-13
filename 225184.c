static void v4l_print_enuminput(const void *arg, bool write_only)
{
	const struct v4l2_input *p = arg;

	pr_cont("index=%u, name=%.*s, type=%u, audioset=0x%x, tuner=%u, std=0x%08Lx, status=0x%x, capabilities=0x%x\n",
		p->index, (int)sizeof(p->name), p->name, p->type, p->audioset,
		p->tuner, (unsigned long long)p->std, p->status,
		p->capabilities);
}