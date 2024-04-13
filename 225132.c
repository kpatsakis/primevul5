static void v4l_print_dbg_register(const void *arg, bool write_only)
{
	const struct v4l2_dbg_register *p = arg;

	pr_cont("type=%u, ", p->match.type);
	if (p->match.type == V4L2_CHIP_MATCH_I2C_DRIVER)
		pr_cont("name=%.*s, ",
				(int)sizeof(p->match.name), p->match.name);
	else
		pr_cont("addr=%u, ", p->match.addr);
	pr_cont("reg=0x%llx, val=0x%llx\n",
			p->reg, p->val);
}