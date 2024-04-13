static void v4l_print_ext_controls(const void *arg, bool write_only)
{
	const struct v4l2_ext_controls *p = arg;
	int i;

	pr_cont("which=0x%x, count=%d, error_idx=%d, request_fd=%d",
			p->which, p->count, p->error_idx, p->request_fd);
	for (i = 0; i < p->count; i++) {
		unsigned int id = p->controls[i].id;
		const char *name = v4l2_ctrl_get_name(id);

		if (name)
			pr_cont(", name=%s", name);
		if (!p->controls[i].size)
			pr_cont(", id/val=0x%x/0x%x", id, p->controls[i].value);
		else
			pr_cont(", id/size=0x%x/%u", id, p->controls[i].size);
	}
	pr_cont("\n");
}