static void v4l_print_cropcap(const void *arg, bool write_only)
{
	const struct v4l2_cropcap *p = arg;

	pr_cont("type=%s, bounds wxh=%dx%d, x,y=%d,%d, defrect wxh=%dx%d, x,y=%d,%d, pixelaspect %d/%d\n",
		prt_names(p->type, v4l2_type_names),
		p->bounds.width, p->bounds.height,
		p->bounds.left, p->bounds.top,
		p->defrect.width, p->defrect.height,
		p->defrect.left, p->defrect.top,
		p->pixelaspect.numerator, p->pixelaspect.denominator);
}