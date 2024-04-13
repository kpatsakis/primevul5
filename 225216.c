static void v4l_print_fmtdesc(const void *arg, bool write_only)
{
	const struct v4l2_fmtdesc *p = arg;

	pr_cont("index=%u, type=%s, flags=0x%x, pixelformat=%c%c%c%c, mbus_code=0x%04x, description='%.*s'\n",
		p->index, prt_names(p->type, v4l2_type_names),
		p->flags, (p->pixelformat & 0xff),
		(p->pixelformat >>  8) & 0xff,
		(p->pixelformat >> 16) & 0xff,
		(p->pixelformat >> 24) & 0xff,
		p->mbus_code,
		(int)sizeof(p->description), p->description);
}