static void v4l_print_querymenu(const void *arg, bool write_only)
{
	const struct v4l2_querymenu *p = arg;

	pr_cont("id=0x%x, index=%d\n", p->id, p->index);
}