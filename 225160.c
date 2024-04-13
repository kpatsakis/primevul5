static void v4l_print_create_buffers(const void *arg, bool write_only)
{
	const struct v4l2_create_buffers *p = arg;

	pr_cont("index=%d, count=%d, memory=%s, ",
			p->index, p->count,
			prt_names(p->memory, v4l2_memory_names));
	v4l_print_format(&p->format, write_only);
}