static void v4l_print_dbg_chip_info(const void *arg, bool write_only)
{
	const struct v4l2_dbg_chip_info *p = arg;

	pr_cont("type=%u, ", p->match.type);
	if (p->match.type == V4L2_CHIP_MATCH_I2C_DRIVER)
		pr_cont("name=%.*s, ",
				(int)sizeof(p->match.name), p->match.name);
	else
		pr_cont("addr=%u, ", p->match.addr);
	pr_cont("name=%.*s\n", (int)sizeof(p->name), p->name);
}