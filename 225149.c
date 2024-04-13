static void v4l_print_query_ext_ctrl(const void *arg, bool write_only)
{
	const struct v4l2_query_ext_ctrl *p = arg;

	pr_cont("id=0x%x, type=%d, name=%.*s, min/max=%lld/%lld, step=%lld, default=%lld, flags=0x%08x, elem_size=%u, elems=%u, nr_of_dims=%u, dims=%u,%u,%u,%u\n",
			p->id, p->type, (int)sizeof(p->name), p->name,
			p->minimum, p->maximum,
			p->step, p->default_value, p->flags,
			p->elem_size, p->elems, p->nr_of_dims,
			p->dims[0], p->dims[1], p->dims[2], p->dims[3]);
}