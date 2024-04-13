static void v4l_print_requestbuffers(const void *arg, bool write_only)
{
	const struct v4l2_requestbuffers *p = arg;

	pr_cont("count=%d, type=%s, memory=%s\n",
		p->count,
		prt_names(p->type, v4l2_type_names),
		prt_names(p->memory, v4l2_memory_names));
}