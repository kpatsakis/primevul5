static void v4l_print_control(const void *arg, bool write_only)
{
	const struct v4l2_control *p = arg;
	const char *name = v4l2_ctrl_get_name(p->id);

	if (name)
		pr_cont("name=%s, ", name);
	pr_cont("id=0x%x, value=%d\n", p->id, p->value);
}