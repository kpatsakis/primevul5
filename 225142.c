static void v4l_print_sliced_vbi_cap(const void *arg, bool write_only)
{
	const struct v4l2_sliced_vbi_cap *p = arg;
	int i;

	pr_cont("type=%s, service_set=0x%08x\n",
			prt_names(p->type, v4l2_type_names), p->service_set);
	for (i = 0; i < 24; i++)
		printk(KERN_DEBUG "line[%02u]=0x%04x, 0x%04x\n", i,
				p->service_lines[0][i],
				p->service_lines[1][i]);
}