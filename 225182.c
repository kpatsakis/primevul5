static void v4l_print_frmsizeenum(const void *arg, bool write_only)
{
	const struct v4l2_frmsizeenum *p = arg;

	pr_cont("index=%u, pixelformat=%c%c%c%c, type=%u",
			p->index,
			(p->pixel_format & 0xff),
			(p->pixel_format >>  8) & 0xff,
			(p->pixel_format >> 16) & 0xff,
			(p->pixel_format >> 24) & 0xff,
			p->type);
	switch (p->type) {
	case V4L2_FRMSIZE_TYPE_DISCRETE:
		pr_cont(", wxh=%ux%u\n",
			p->discrete.width, p->discrete.height);
		break;
	case V4L2_FRMSIZE_TYPE_STEPWISE:
		pr_cont(", min=%ux%u, max=%ux%u, step=%ux%u\n",
				p->stepwise.min_width,
				p->stepwise.min_height,
				p->stepwise.max_width,
				p->stepwise.max_height,
				p->stepwise.step_width,
				p->stepwise.step_height);
		break;
	case V4L2_FRMSIZE_TYPE_CONTINUOUS:
	default:
		pr_cont("\n");
		break;
	}
}