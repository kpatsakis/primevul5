static void v4l_print_crop(const void *arg, bool write_only)
{
	const struct v4l2_crop *p = arg;

	pr_cont("type=%s, wxh=%dx%d, x,y=%d,%d\n",
		prt_names(p->type, v4l2_type_names),
		p->c.width, p->c.height,
		p->c.left, p->c.top);
}