static void v4l_print_frmivalenum(const void *arg, bool write_only)
{
	const struct v4l2_frmivalenum *p = arg;

	pr_cont("index=%u, pixelformat=%c%c%c%c, wxh=%ux%u, type=%u",
			p->index,
			(p->pixel_format & 0xff),
			(p->pixel_format >>  8) & 0xff,
			(p->pixel_format >> 16) & 0xff,
			(p->pixel_format >> 24) & 0xff,
			p->width, p->height, p->type);
	switch (p->type) {
	case V4L2_FRMIVAL_TYPE_DISCRETE:
		pr_cont(", fps=%d/%d\n",
				p->discrete.numerator,
				p->discrete.denominator);
		break;
	case V4L2_FRMIVAL_TYPE_STEPWISE:
		pr_cont(", min=%d/%d, max=%d/%d, step=%d/%d\n",
				p->stepwise.min.numerator,
				p->stepwise.min.denominator,
				p->stepwise.max.numerator,
				p->stepwise.max.denominator,
				p->stepwise.step.numerator,
				p->stepwise.step.denominator);
		break;
	case V4L2_FRMIVAL_TYPE_CONTINUOUS:
	default:
		pr_cont("\n");
		break;
	}
}