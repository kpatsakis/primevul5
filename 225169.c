static void v4l_print_enum_dv_timings(const void *arg, bool write_only)
{
	const struct v4l2_enum_dv_timings *p = arg;

	pr_cont("index=%u, ", p->index);
	v4l_print_dv_timings(&p->timings, write_only);
}