static void v4l_print_edid(const void *arg, bool write_only)
{
	const struct v4l2_edid *p = arg;

	pr_cont("pad=%u, start_block=%u, blocks=%u\n",
		p->pad, p->start_block, p->blocks);
}