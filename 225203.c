static void v4l_print_selection(const void *arg, bool write_only)
{
	const struct v4l2_selection *p = arg;

	pr_cont("type=%s, target=%d, flags=0x%x, wxh=%dx%d, x,y=%d,%d\n",
		prt_names(p->type, v4l2_type_names),
		p->target, p->flags,
		p->r.width, p->r.height, p->r.left, p->r.top);
}