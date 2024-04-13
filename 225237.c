static void v4l_print_frequency(const void *arg, bool write_only)
{
	const struct v4l2_frequency *p = arg;

	pr_cont("tuner=%u, type=%u, frequency=%u\n",
				p->tuner, p->type, p->frequency);
}