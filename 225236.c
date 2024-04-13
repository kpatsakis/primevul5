static void v4l_print_exportbuffer(const void *arg, bool write_only)
{
	const struct v4l2_exportbuffer *p = arg;

	pr_cont("fd=%d, type=%s, index=%u, plane=%u, flags=0x%08x\n",
		p->fd, prt_names(p->type, v4l2_type_names),
		p->index, p->plane, p->flags);
}