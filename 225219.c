static void v4l_print_dv_timings_cap(const void *arg, bool write_only)
{
	const struct v4l2_dv_timings_cap *p = arg;

	switch (p->type) {
	case V4L2_DV_BT_656_1120:
		pr_cont("type=bt-656/1120, width=%u-%u, height=%u-%u, pixelclock=%llu-%llu, standards=0x%x, capabilities=0x%x\n",
			p->bt.min_width, p->bt.max_width,
			p->bt.min_height, p->bt.max_height,
			p->bt.min_pixelclock, p->bt.max_pixelclock,
			p->bt.standards, p->bt.capabilities);
		break;
	default:
		pr_cont("type=%u\n", p->type);
		break;
	}
}